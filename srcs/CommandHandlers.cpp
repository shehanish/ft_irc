/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 08:39:57 by spitul            #+#    #+#             */
/*   Updated: 2025/10/08 17:09:35 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void	Server::handleJoin(Client &client, const std::vector<std::string> &args)
{
	if (args.empty())
		return; // :server 461 <nick> JOIN :Not enough parameters
	Channel	*channel;
	
	if (args.empty())
		return; //message
	if (client.getNbChannel() > MAX_CHANNELS)
		return; //send message
	channel = getChannel(args[0]);
	if (channel == NULL)
	{
		channel = createChannel(args[0], client);
	}
	else
	{
		if (channel->hasKey() && !channel->checkKey(args[1]))
			return;
		if (!channel->isInvited(client))
			return;
		if (channel->hasLimit().active && channel->getMembers().size() >= static_cast<size_t>(channel->getLimit()))
		;
			return; // ERR_CHANNELISFULL (471) 
		channel->addUser(client);
		client.addChannel();
		//send messages + topic
	}
}

void	Server::handlePart(Client &client, const std::vector<std::string> &args)
{
	if (args.empty())
		return;
	Channel	*channel;
	std::vector<std::string>::const_iterator	it = args.begin();
	for (; it != args.end(); it ++)
	{
		channel = getChannel(*it);
		if (!channel->isMember(client))
		{
			//send msg
			continue;
		}
		channel->delUser(client);
		
		//send part message
	}
		
}

void	Server::handlePrivMsg(Client &client, const std::vector<std::string> &args)
{
	if (!args.empty())
		return; // :server 461 <nick> PRIVMSG :Not enough parameters
	const std::string	*msg = client.getMsg(args);
	std::vector<std::string>::const_iterator	it = args.begin();
	while (it != args.end())
	{
		if ((*it)[0] == '#')
		{
			Channel	*channel = getChannel(*it);
			if (channel == NULL)
				{}//401 ERR_NOSUCHNICK "<nickname> :No such nick/channel"
			broadcastMsg(client, channel, *msg);
		}
		else
		{
			Client	*target = getUser(*it);
			if (target == NULL)
				{} //401 ERR_NOSUCHNICK "<nickname> :No such nick/channel"
			client.sendMsg(*target, *msg);
		}
	}
}

// KICK <channel> <user> [<comment>]

void	Server::handleKick(Client &client, const std::vector<std::string> &args)
{
	//KICK <channel>{,<channel>} <user>{,<user>} [<comment>]
	std::vector<Client*>	users = getUserArguments(args);
	std::vector<Channel*>	channels = getChanArguments(args);
	
	// const std::string *msg = 
	client.getMsg(args);
	
	size_t	i = 0;
	size_t	j = 0;
	while (i < channels.size() && j < users.size())
	{
		channels[i++]->delUser(*(users[j++]));
					
	}
	if (i == channels.size() && j < users.size())
	{
		while (j < users.size())
			channels[i]->delUser(*(users[j++]));
	}
	else if (j == users.size() && i < channels.size())
	{
		while (i < channels.size())
			channels[i]->delUser(*(users[j]));
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
	if (channel->isMember(client))
		return; // 442 <channel> :You're not on that channel
	if (channel->isMember(*user))
		return; // 443 ERR_USERONCHANNEL <nick> <channel> :is already on channel
	if (channel->isInviteOnly() && !channel->isOperator(client))
		return; // 482 ERR_CHANOPRIVSNEEDED <channel>
	channel->addUser(*user);
	if (channel->isInviteOnly())
		channel->addInvitedMember(*user);
}

void	Server::handleTopic(Client &client, const std::vector<std::string> &args)
{
	size_t	size = args.size();
	if (size > 2)
		return; // err
	Channel	*channel = getChannel(args[0]); 
	if (!channel)
		return; // 403 ERR_NOSUCHCHANNEL <channel> :No such channel
	if (!channel->isMember(client))
		return; // 442 ERR_NOTONCHANNEL <channel> :You're not on that channel
	if (size == 2)
	{
		if (channel->hasRestrictedTopic() && !channel->isOperator(client))
			return; // 482 ERR_CHANOPRIVSNEEDED <channel> :You're not channel operator
		channel->setTopic(args[1]); 
	} 
	else if (size == 1)
	{
		if (channel->getTopic().empty())
			client.sendMsg(client, ":server.name 331 <nick> #channel :No topic is set"); //to do
	}
	broadcastMsg(client, channel, ":alice!~alice@host TOPIC #school :Homework due Monday!"); // to do
}

static std::vector<std::string>	getModeParams(const std::vector<std::string> &args)
{
	std::vector<std::string>	params;
	for (int i = 1; i < args.size(); i++)
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
	int	indexParams = 0;
	for (int i = 1; i < args.size(); i++)
	{
		std::string::const_iterator	it = args[i].begin();
		for (it; it != args[i].end(); it ++)
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
	
}
