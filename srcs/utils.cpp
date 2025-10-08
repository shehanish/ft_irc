/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 18:02:35 by spitul            #+#    #+#             */
/*   Updated: 2025/10/08 17:11:50 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

const std::vector<Client*>	Server::getUserArguments(const std::vector<std::string> &args)
{
	std::vector<Client*>	users;
	for(size_t	i = 0; i < args.size(); i++)
	{
		if (args[i][0] != '#' && args[i][0] != ':')
		{
			Client *new_user = getUser(args[i]);
			if (new_user != NULL)
				users.push_back(new_user);
			else
				; // prob error msg?
		}
	}
	return users;
}

const std::vector<Channel*>	Server::getChanArguments(const std::vector<std::string> &args)
{
	std::vector<Channel*>	channels;
	for(size_t	i = 0; i < args.size(); i++)
	{
		if (args[i][0] == '#')
		{
			Channel	*new_channel = getChannel(args[i]);
			if (new_channel != NULL)
				channels.push_back(new_channel);
		}
	}
	return channels;
}
