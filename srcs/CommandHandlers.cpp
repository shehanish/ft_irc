/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shkaruna <shkaruna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 08:39:57 by spitul            #+#    #+#             */
/*   Updated: 2025/10/13 16:38:01 by shkaruna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void	Server::handleJoin(Client &client, const std::vector<std::string> &args)
{
	if (args.empty())
	{
		std::string error = ":localhost 461 " + client.getNick() + " JOIN :Not enough parameters\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	if (client.getNbChannel() >= MAX_CHANNELS)
	{
		std::string error = ":localhost 405 " + client.getNick() + " " + args[0] + " :You have joined too many channels\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	std::string channelName = args[0];
	Channel	*channel = getChannel(channelName);
	
	if (channel == NULL)
	{
		// Create new channel
		channel = createChannel(channelName, client);
		client.addChannel();
		
		// Send JOIN confirmation to the client
		std::string joinMsg = ":" + client.getNick() + "!~" + client.getUserName() + "@localhost JOIN :" + channelName + "\r\n";
		client.appendToSendBuffer(joinMsg);
		
		// Send topic (empty for new channel)
		std::string topicMsg = ":localhost 331 " + client.getNick() + " " + channelName + " :No topic is set\r\n";
		client.appendToSendBuffer(topicMsg);
		
		// Send NAMES list (just the creator for now)
		std::string namesMsg = ":localhost 353 " + client.getNick() + " = " + channelName + " :@" + client.getNick() + "\r\n";
		client.appendToSendBuffer(namesMsg);
		std::string endNames = ":localhost 366 " + client.getNick() + " " + channelName + " :End of /NAMES list\r\n";
		client.appendToSendBuffer(endNames);
	}
	else
	{
		// Join existing channel - check permissions
		if (channel->hasKey() && (args.size() < 2 || !channel->checkKey(args[1])))
		{
			std::string error = ":localhost 475 " + client.getNick() + " " + channelName + " :Cannot join channel (+k) - bad key\r\n";
			client.appendToSendBuffer(error);
			return;
		}
		if (!channel->isInvited(client) && channel->isInviteOnly())
		{
			std::string error = ":localhost 473 " + client.getNick() + " " + channelName + " :Cannot join channel (+i)\r\n";
			client.appendToSendBuffer(error);
			return;
		}
		if (channel->hasLimit().active && channel->getMembers().size() >= static_cast<size_t>(channel->getLimit()))
		{
			std::string error = ":localhost 471 " + client.getNick() + " " + channelName + " :Cannot join channel (+l)\r\n";
			client.appendToSendBuffer(error);
			return;
		}
		
		// Broadcast JOIN to all existing members
		std::string joinMsg = ":" + client.getNick() + "!~" + client.getUserName() + "@localhost JOIN :" + channelName + "\r\n";
		std::set<Client*>::iterator it = channel->getMembers().begin();
		for (; it != channel->getMembers().end(); it++)
		{
			(*it)->appendToSendBuffer(joinMsg);
		}
		
		// Add user to channel
		channel->addUser(client);
		client.addChannel();
		
		// Send JOIN to the joining client too
		client.appendToSendBuffer(joinMsg);
		
		// Send topic if set
		if (!channel->getTopic().empty())
		{
			std::string topicMsg = ":localhost 332 " + client.getNick() + " " + channelName + " :" + channel->getTopic() + "\r\n";
			client.appendToSendBuffer(topicMsg);
		}
		else
		{
			std::string topicMsg = ":localhost 331 " + client.getNick() + " " + channelName + " :No topic is set\r\n";
			client.appendToSendBuffer(topicMsg);
		}
		
		// Send NAMES list
		std::string namesList = ":localhost 353 " + client.getNick() + " = " + channelName + " :";
		std::set<Client*>::iterator nit = channel->getMembers().begin();
		for (; nit != channel->getMembers().end(); nit++)
		{
			if (channel->isOperator(**nit))
				namesList += "@";
			namesList += (*nit)->getNick() + " ";
		}
		namesList += "\r\n";
		client.appendToSendBuffer(namesList);
		
		std::string endNames = ":localhost 366 " + client.getNick() + " " + channelName + " :End of /NAMES list\r\n";
		client.appendToSendBuffer(endNames);
	}
}

void	Server::handlePart(Client &client, const std::vector<std::string> &args)
{
	if (args.empty())
	{
		std::string error = ":localhost 461 " + client.getNick() + " PART :Not enough parameters\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	std::vector<std::string>::const_iterator	it = args.begin();
	for (; it != args.end(); it++)
	{
		std::string channelName = *it;
		Channel *channel = getChannel(channelName);
		
		if (!channel)
		{
			std::string error = ":localhost 403 " + client.getNick() + " " + channelName + " :No such channel\r\n";
			client.appendToSendBuffer(error);
			continue;
		}
		if (!channel->isMember(client))
		{
			std::string error = ":localhost 442 " + client.getNick() + " " + channelName + " :You're not on that channel\r\n";
			client.appendToSendBuffer(error);
			continue;
		}
		
		// Build PART message
		std::string partMsg = ":" + client.getNick() + "!~" + client.getUserName() + "@localhost PART " + channelName;
		
		// Add part message if provided (look for trailing parameter starting with :)
		if (args.size() > 1 && args.back()[0] == ':')
		{
			partMsg += " " + args.back();
		}
		partMsg += "\r\n";
		
		// Broadcast PART to all channel members (including the leaving user)
		std::set<Client*>::iterator mit = channel->getMembers().begin();
		for (; mit != channel->getMembers().end(); mit++)
		{
			(*mit)->appendToSendBuffer(partMsg);
		}
		
		// Remove user from channel
		channel->delUser(client);
	}
}

void	Server::handlePrivMsg(Client &client, const std::vector<std::string> &args)
{
	if (args.empty())
	{
		std::string error = ":localhost 461 " + client.getNick() + " PRIVMSG :Not enough parameters\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// Get the message text (last argument starting with :)
	const std::string *msgPtr = client.getMsg(args);
	if (!msgPtr || msgPtr->empty())
	{
		std::string error = ":localhost 412 " + client.getNick() + " :No text to send\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// Remove the leading ':' from the message
	std::string message = *msgPtr;
	if (message[0] == ':')
		message = message.substr(1);
	
	// Iterate through all targets (can send to multiple channels/users)
	std::vector<std::string>::const_iterator it = args.begin();
	while (it != args.end())
	{
		// Skip if this is the message itself
		if ((*it)[0] == ':')
		{
			it++;
			continue;
		}
		
		if ((*it)[0] == '#')
		{
			// Channel message
			std::string channelName = *it;
			Channel	*channel = getChannel(channelName);
			if (channel == NULL)
			{
				std::string error = ":localhost 401 " + client.getNick() + " " + channelName + " :No such nick/channel\r\n";
				client.appendToSendBuffer(error);
				it++;
				continue;
			}
			
			// Check if user is in the channel
			if (!channel->isMember(client))
			{
				std::string error = ":localhost 404 " + client.getNick() + " " + channelName + " :Cannot send to channel\r\n";
				client.appendToSendBuffer(error);
				it++;
				continue;
			}
			
			// Format the message with proper IRC syntax
			std::string formattedMsg = ":" + client.getNick() + "!~" + client.getUserName() + "@localhost PRIVMSG " + channelName + " :" + message + "\r\n";
			
			std::cout << "DEBUG: Sending message from " << client.getNick() << " to channel " << channelName << std::endl;
			std::cout << "DEBUG: Channel has " << channel->getMembers().size() << " members" << std::endl;
			std::cout << "DEBUG: Message: " << formattedMsg << std::endl;
			
			// Send to all channel members EXCEPT the sender
			std::set<Client*>::iterator sit = channel->getMembers().begin();
			for (; sit != channel->getMembers().end(); sit++)
			{
				std::cout << "DEBUG: Checking member " << (*sit)->getNick() << std::endl;
				if (*sit != &client) // Don't echo back to sender
				{
					std::cout << "DEBUG: Adding to send buffer for " << (*sit)->getNick() << std::endl;
					(*sit)->appendToSendBuffer(formattedMsg);
				}
			}
		}
		else
		{
			// Private message to user
			std::string targetNick = *it;
			Client *target = getUser(targetNick);
			if (target == NULL)
			{
				std::string error = ":localhost 401 " + client.getNick() + " " + targetNick + " :No such nick/channel\r\n";
				client.appendToSendBuffer(error);
				it++;
				continue;
			}
			
			// Format the message with proper IRC syntax
			std::string formattedMsg = ":" + client.getNick() + "!~" + client.getUserName() + "@localhost PRIVMSG " + targetNick + " :" + message + "\r\n";
			target->appendToSendBuffer(formattedMsg);
		}
		it++;
	}
}

// KICK <channel> <user> [<comment>]

void	Server::handleKick(Client &client, const std::vector<std::string> &args)
{
	//KICK <channel>{,<channel>} <user>{,<user>} [<comment>]
	if (args.size() < 2)
		return; // 461 ERR_NEEDMOREPARAMS
	
	std::vector<Client*>	users = getUserArguments(args);
	std::vector<Channel*>	channels = getChanArguments(args);
	
	if (users.empty() || channels.empty())
		return;
	
	// const std::string *msg = 
	client.getMsg(args);
	
	size_t	i = 0;
	size_t	j = 0;
	while (i < channels.size() && j < users.size())
	{
		if (channels[i] && users[j])
			channels[i++]->delUser(*(users[j++]));
		else
			break;
	}
	if (i == channels.size() && j < users.size())
	{
		while (j < users.size() && channels[i - 1])
			channels[i - 1]->delUser(*(users[j++]));
	}
	else if (j == users.size() && i < channels.size())
	{
		while (i < channels.size() && users[j - 1])
			channels[i++]->delUser(*(users[j - 1]));
	}
}

void	Server::handleInvite(Client &client, const std::vector<std::string> &args)
{
	// INVITE <nickname> <channel>
	if (args.size() != 2)
		return; //send error msg
	Client	*user = getUser(args[0]);
	if (!user)
		return; // 401 ERR_NOSUCHNICK <nickname> :No such nick
	Channel *channel = getChannel(args[1]);
	if (!channel)
		return; // 403 <channel> :No such channel
	if (!channel->isMember(client))
		return; // 442 <channel> :You're not on that channel
	if (channel->isMember(*user))
		return; // 443 ERR_USERONCHANNEL <nick> <channel> :is already on channel
	if (channel->isInviteOnly() && !channel->isOperator(client))
		return; // 482 ERR_CHANOPRIVSNEEDED <channel>
	
	if (channel->isInviteOnly())
		channel->addInvitedMember(*user);
	// Send invite notification to user
}

void	Server::handleTopic(Client &client, const std::vector<std::string> &args)
{
	if (args.empty())
		return; // 461 ERR_NEEDMOREPARAMS
	
	size_t	size = args.size();
	Channel	*channel = getChannel(args[0]); 
	if (!channel)
		return; // 403 ERR_NOSUCHCHANNEL <channel> :No such channel
	if (!channel->isMember(client))
		return; // 442 ERR_NOTONCHANNEL <channel> :You're not on that channel
	if (size >= 2)
	{
		if (channel->hasRestrictedTopic() && !channel->isOperator(client))
			return; // 482 ERR_CHANOPRIVSNEEDED <channel> :You're not channel operator
		channel->setTopic(args[1]); 
		broadcastMsg(client, channel, ":alice!~alice@host TOPIC #school :Homework due Monday!"); // to do
	} 
	else if (size == 1)
	{
		if (channel->getTopic().empty())
			client.sendMsg(client, ":server.name 331 <nick> #channel :No topic is set"); //to do
		else
		{
			// Send topic to client
			std::string topic_msg = ":server.name 332 " + client.getNick() + " " + args[0] + " :" + channel->getTopic() + "\r\n";
			client.appendToSendBuffer(topic_msg);
		}
	}
}

/* static std::vector<std::string>	getModeParams(const std::vector<std::string> &args)
{
	std::vector<std::string>	params;
	for (size_t i = 1; i < args.size(); i++)
	{
		if (args[i][0] != '+' && args[i][0] != '-')
			params.push_back(args[i]);
	}
	return params;
}

void	Server::handleMode(Client &client, const std::vector<std::string> &args)
{
	bool	adding = false;
	
	Channel *channel = getChannel(args[0]);
	(void)client;
	if (!channel)
		return; // no such channel
	if (!channel->isOperator(client))
		return; // 482 ERR_CHANOPRIVSNEEDED
	std::vector<std::string>	params = getModeParams(args);
	size_t	indexParams = 0;
	for (size_t i = 1; i < args.size(); i++)
	{
		std::string::const_iterator	it = args[i].begin();
		for (; it != args[i].end(); it ++)
		{
			char	c = *it;
			if (c == '+')
			{
				adding = true;
				continue;
			}
			if (c == '-')
			{
				adding = false;
				continue;
			}
			switch(c) //itkol
			{
				case 'i':
					channel->setInviteOnly(adding);
					break;
				case 't':
					channel->restrictTopic(adding);
					break;
				case 'k':
					if (adding)
					{
						if (indexParams < params.size() && !params[indexParams].empty())
							channel->setKey(params[indexParams++]);
						else
							return; // 461 ERR_NEEDMOREPARAMS ERR_INVALIDMODEPARAM (696) 
					}
					else
					{
						if (indexParams < params.size())
						{
							if (channel->checkKey(params[indexParams]))
							{
								channel->setKey("");
								indexParams++;
							}
							else
								return; // ERR_INVALIDKEY (525)
						}
					}
			}
		}
		
	}
	
} */

// WHO <channel>
// Returns information about users in a channel
void	Server::handleWho(Client &client, const std::vector<std::string> &args)
{
	if (args.empty())
	{
		// WHO without parameters - not commonly implemented, just return end of WHO
		std::string reply = ":localhost 315 " + client.getNick() + " * :End of WHO list\r\n";
		client.appendToSendBuffer(reply);
		return;
	}
	
	std::string target = args[0];
	
	// Check if it's a channel
	if (target[0] == '#')
	{
		Channel *channel = getChannel(target);
		if (channel == NULL)
		{
			// Channel doesn't exist - send end of WHO
			std::string reply = ":localhost 315 " + client.getNick() + " " + target + " :End of WHO list\r\n";
			client.appendToSendBuffer(reply);
			return;
		}
		
		// Send WHO reply for each member in the channel
		// Format: :server 352 <client> <channel> <user> <host> <server> <nick> <flags> :<hopcount> <realname>
		std::set<Client*>::iterator it = channel->getMembers().begin();
		for (; it != channel->getMembers().end(); it++)
		{
			Client *member = *it;
			std::string flags = "H"; // H = here (not away)
			if (channel->isOperator(*member))
				flags += "@";
			
			std::string reply = ":localhost 352 " + client.getNick() + " " + target + " " +
								member->getUserName() + " localhost localhost " + member->getNick() +
								" " + flags + " :0 " + member->getUserName() + "\r\n";
			client.appendToSendBuffer(reply);
		}
		
		// Send end of WHO list
		std::string endReply = ":localhost 315 " + client.getNick() + " " + target + " :End of WHO list\r\n";
		client.appendToSendBuffer(endReply);
	}
	else
	{
		// Single user query
		Client *targetUser = getUser(target);
		if (targetUser != NULL)
		{
			std::string reply = ":localhost 352 " + client.getNick() + " * " +
								targetUser->getUserName() + " localhost localhost " + targetUser->getNick() +
								" H :0 " + targetUser->getUserName() + "\r\n";
			client.appendToSendBuffer(reply);
		}
		
		// Send end of WHO list
		std::string endReply = ":localhost 315 " + client.getNick() + " " + target + " :End of WHO list\r\n";
		client.appendToSendBuffer(endReply);
	}
}
