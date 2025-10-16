/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 08:39:57 by spitul            #+#    #+#             */
/*   Updated: 2025/10/16 14:10:02 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <sstream>
#include <cstdlib>

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
	
	if (args.size() < 2)
	{
		std::string error = ":localhost 412 " + client.getNick() + " :No text to send\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// The message is the last argument (parse() already removed the ':')
	std::string message = args.back();
	if (message.empty())
	{
		std::string error = ":localhost 412 " + client.getNick() + " :No text to send\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// Iterate through all targets (can send to multiple channels/users)
	// All args except the last one are targets
	for (size_t i = 0; i < args.size() - 1; i++)
	{
		std::string target = args[i];
		
		if (target[0] == '#')
		{
			// Channel message
			Channel	*channel = getChannel(target);
			if (channel == NULL)
			{
				std::string error = ":localhost 401 " + client.getNick() + " " + target + " :No such nick/channel\r\n";
				client.appendToSendBuffer(error);
				continue;
			}
			
			// Check if user is in the channel
			if (!channel->isMember(client))
			{
				std::string error = ":localhost 404 " + client.getNick() + " " + target + " :Cannot send to channel\r\n";
				client.appendToSendBuffer(error);
				continue;
			}
			
			// Format the message with proper IRC syntax
			std::string formattedMsg = ":" + client.getNick() + "!~" + client.getUserName() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";
			
			// Send to all channel members EXCEPT the sender
			std::set<Client*>::iterator sit = channel->getMembers().begin();
			for (; sit != channel->getMembers().end(); sit++)
			{
				if (*sit != &client) // Don't echo back to sender
					(*sit)->appendToSendBuffer(formattedMsg);
			}
		}
		else
		{
			// Private message to user
			Client *targetClient = getUser(target);
			if (targetClient == NULL)
			{
				std::string error = ":localhost 401 " + client.getNick() + " " + target + " :No such nick/channel\r\n";
				client.appendToSendBuffer(error);
				continue;
			}
			
			// Format the message with proper IRC syntax
			std::string formattedMsg = ":" + client.getNick() + "!~" + client.getUserName() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";
			targetClient->appendToSendBuffer(formattedMsg);
		}
	}
}

// KICK <channel> <user> [<comment>]
void	Server::handleKick(Client &client, const std::vector<std::string> &args)
{
	if (args.size() < 2)
	{
		std::string error = ":localhost 461 " + client.getNick() + " KICK :Not enough parameters\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	std::string channelName = args[0];
	std::string targetNick = args[1];
	std::string reason = (args.size() >= 3) ? args[2] : client.getNick();
	
	// Get the channel
	Channel *channel = getChannel(channelName);
	if (!channel)
	{
		std::string error = ":localhost 403 " + client.getNick() + " " + channelName + " :No such channel\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// Check if the kicker is on the channel
	if (!channel->isMember(client))
	{
		std::string error = ":localhost 442 " + client.getNick() + " " + channelName + " :You're not on that channel\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// Check if the kicker is an operator
	if (!channel->isOperator(client))
	{
		std::string error = ":localhost 482 " + client.getNick() + " " + channelName + " :You're not channel operator\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// Get the target user
	Client *target = getUser(targetNick);
	if (!target)
	{
		std::string error = ":localhost 401 " + client.getNick() + " " + targetNick + " :No such nick/channel\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// Check if target is on the channel
	if (!channel->isMember(*target))
	{
		std::string error = ":localhost 441 " + client.getNick() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// Broadcast KICK message to all channel members (including the kicked user)
	std::string kickMsg = ":" + client.getNick() + "!~" + client.getUserName() + "@localhost KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
	
	std::set<Client*>::iterator it = channel->getMembers().begin();
	for (; it != channel->getMembers().end(); it++)
		(*it)->appendToSendBuffer(kickMsg);
	
	// Remove the user from the channel
	channel->delUser(*target);
}

void	Server::handleInvite(Client &client, const std::vector<std::string> &args)
{
	// INVITE <nickname> <channel>
	if (args.size() != 2)
	{
		std::string error = ":localhost 461 " + client.getNick() + " INVITE :Not enough parameters\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	std::string targetNick = args[0];
	std::string channelName = args[1];
	
	Client *user = getUser(targetNick);
	if (!user)
	{
		std::string error = ":localhost 401 " + client.getNick() + " " + targetNick + " :No such nick/channel\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	Channel *channel = getChannel(channelName);
	if (!channel)
	{
		std::string error = ":localhost 403 " + client.getNick() + " " + channelName + " :No such channel\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	if (!channel->isMember(client))
	{
		std::string error = ":localhost 442 " + client.getNick() + " " + channelName + " :You're not on that channel\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	if (channel->isMember(*user))
	{
		std::string error = ":localhost 443 " + client.getNick() + " " + targetNick + " " + channelName + " :is already on channel\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	if (channel->isInviteOnly() && !channel->isOperator(client))
	{
		std::string error = ":localhost 482 " + client.getNick() + " " + channelName + " :You're not channel operator\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// Add user to invite list if channel is invite-only
	if (channel->isInviteOnly())
		channel->addInvitedMember(*user);
	
	// Send invite notification to the invited user
	std::string inviteMsg = ":" + client.getNick() + "!~" + client.getUserName() + "@localhost INVITE " + targetNick + " :" + channelName + "\r\n";
	user->appendToSendBuffer(inviteMsg);
	
	// Confirm to the inviter
	std::string confirmMsg = ":localhost 341 " + client.getNick() + " " + targetNick + " " + channelName + "\r\n";
	client.appendToSendBuffer(confirmMsg);
}

void	Server::handleTopic(Client &client, const std::vector<std::string> &args)
{
	if (args.empty())
	{
		std::string error = ":localhost 461 " + client.getNick() + " TOPIC :Not enough parameters\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	std::string channelName = args[0];
	Channel *channel = getChannel(channelName);
	
	if (!channel)
	{
		std::string error = ":localhost 403 " + client.getNick() + " " + channelName + " :No such channel\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	if (!channel->isMember(client))
	{
		std::string error = ":localhost 442 " + client.getNick() + " " + channelName + " :You're not on that channel\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// Setting topic
	if (args.size() >= 2)
	{
		if (channel->hasRestrictedTopic() && !channel->isOperator(client))
		{
			std::string error = ":localhost 482 " + client.getNick() + " " + channelName + " :You're not channel operator\r\n";
			client.appendToSendBuffer(error);
			return;
		}
		
		// Set the new topic
		std::string newTopic = args[1];
		channel->setTopic(newTopic);
		
		// Broadcast topic change to all channel members
		std::string topicMsg = ":" + client.getNick() + "!~" + client.getUserName() + "@localhost TOPIC " + channelName + " :" + newTopic + "\r\n";
		std::set<Client*>::iterator it = channel->getMembers().begin();
		for (; it != channel->getMembers().end(); it++)
			(*it)->appendToSendBuffer(topicMsg);
	}
	// Viewing topic
	else
	{
		if (channel->getTopic().empty())
		{
			std::string noTopicMsg = ":localhost 331 " + client.getNick() + " " + channelName + " :No topic is set\r\n";
			client.appendToSendBuffer(noTopicMsg);
		}
		else
		{
			std::string topicMsg = ":localhost 332 " + client.getNick() + " " + channelName + " :" + channel->getTopic() + "\r\n";
			client.appendToSendBuffer(topicMsg);
		}
	}
}

// MODE <channel> [<modes> [<mode params>]]
// MODE #test - view current modes
// MODE #test +i - set invite-only
// MODE #test +o nick - give operator status to nick
// MODE #test +k password - set channel password
// MODE #test +l 10 - set user limit to 10
void	Server::handleMode(Client &client, const std::vector<std::string> &args)
{
	if (args.empty())
	{
		std::string error = ":localhost 461 " + client.getNick() + " MODE :Not enough parameters\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	std::string target = args[0];
	
	// Only support channel modes for now
	if (target[0] != '#')
	{
		// User modes not implemented
		return;
	}
	
	Channel *channel = getChannel(target);
	if (!channel)
	{
		std::string error = ":localhost 403 " + client.getNick() + " " + target + " :No such channel\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// If only channel name provided, show current modes
	if (args.size() == 1)
	{
		std::string modes = "+";
		std::string params = "";
		
		if (channel->isInviteOnly())
			modes += "i";
		if (channel->hasRestrictedTopic())
			modes += "t";
		if (channel->hasKey())
		{
			modes += "k";
			params += " *"; // Don't reveal actual password
		}
		if (channel->hasLimit().active)
		{
			modes += "l";
			std::stringstream ss;
			ss << channel->getLimit();
			params += " " + ss.str();
		}
		
		std::string modeMsg = ":localhost 324 " + client.getNick() + " " + target + " " + modes + params + "\r\n";
		client.appendToSendBuffer(modeMsg);
		return;
	}
	
	// Check if user is operator (only ops can change modes)
	if (!channel->isOperator(client))
	{
		std::string error = ":localhost 482 " + client.getNick() + " " + target + " :You're not channel operator\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// Parse mode changes
	bool adding = true;
	std::string modeChanges = "";
	std::string modeParams = "";
	size_t paramIndex = 2; // Parameters start at args[2]
	
	for (size_t i = 0; i < args[1].length(); i++)
	{
		char c = args[1][i];
		
		if (c == '+')
		{
			adding = true;
			continue;
		}
		else if (c == '-')
		{
			adding = false;
			continue;
		}
		
		switch(c)
		{
			case 'i': // Invite-only
				channel->setInviteOnly(adding);
				modeChanges += c;
				break;
				
			case 't': // Topic restriction
				channel->restrictTopic(adding);
				modeChanges += c;
				break;
				
			case 'k': // Channel key (password)
				if (adding)
				{
					if (paramIndex < args.size())
					{
						channel->setKey(args[paramIndex]);
						modeChanges += c;
						modeParams += " " + args[paramIndex];
						paramIndex++;
					}
				}
				else
				{
					channel->setKey("");
					modeChanges += c;
				}
				break;
				
			case 'o': // Operator privilege
				if (paramIndex < args.size())
				{
					Client *target = getUser(args[paramIndex]);
					if (target && channel->isMember(*target))
					{
						if (adding)
							channel->addOperator(*target);
						else
							channel->delOperator(*target);
						modeChanges += c;
						modeParams += " " + args[paramIndex];
					}
					paramIndex++;
				}
				break;
				
			case 'l': // User limit
				if (adding)
				{
					if (paramIndex < args.size())
					{
						int limit = atoi(args[paramIndex].c_str());
						if (limit > 0)
						{
							channel->setLimit(limit);
							modeChanges += c;
							modeParams += " " + args[paramIndex];
						}
						paramIndex++;
					}
				}
				else
				{
					int zero = 0;
					channel->setLimit(zero);
					modeChanges += c;
				}
				break;
		}
	}
	
	// Broadcast mode change to all channel members
	if (!modeChanges.empty())
	{
		std::string modeMsg = ":" + client.getNick() + "!~" + client.getUserName() + "@localhost MODE " + target + " " + 
							  (adding ? "+" : "-") + modeChanges + modeParams + "\r\n";
		
		std::set<Client*>::iterator it = channel->getMembers().begin();
		for (; it != channel->getMembers().end(); it++)
			(*it)->appendToSendBuffer(modeMsg);
	}
}

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
