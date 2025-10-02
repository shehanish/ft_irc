/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 18:02:35 by spitul            #+#    #+#             */
/*   Updated: 2025/10/02 19:21:48 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

const std::vector<Client*>	Server::getUserArguments(const std::vector<std::string> &args)
{
	std::vector<Client*>	users;
	for(int	i = 0; i < args.size(); i++)
	{
		if (args[i][0] != '#' && args[i][0] != ':')
		{
			Client *user = getUser(args[i]);
			if (user != NULL)
				users.push_back(user);
			
		}
	}
	return users;
}

const std::vector<Channel*>	Server::getChanArguments(const std::vector<std::string> &args)
{
	std::vector<Channel*>	channels;
	for(int	i = 0; i < args.size(); i++)
	{
		if (args[i][0] == '#')
		{
			Channel	*channel = getChannel(args[i]);
			if (channel != NULL)
				channels.push_back(channel);
		}
	}
	return channels;
}
