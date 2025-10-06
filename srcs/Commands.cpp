/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 17:28:05 by spitul            #+#    #+#             */
/*   Updated: 2025/10/06 17:16:28 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void JoinCmd::execute(Server &server, Client &client, const std::vector<std::string> &args)
{
	server.handleJoin(client, args);
}

void PartCmd::execute(Server &server, Client &client, const std::vector<std::string> &args)
{
	server.handlePart(client, args);
}

void PrivMsgCmd::execute(Server &server, Client &client, const std::vector<std::string> &args)
{
	server.handlePrivMsg(client, args);
}

void KickCmd::execute(Server &server, Client &client, const std::vector<std::string> &args)
{
	server.handleKick(client, args);
}

void InviteCmd::execute(Server &server, Client &client, const std::vector<std::string> &args)
{
	server.handleInvite(client, args);
}

void TopicCmd::execute(Server &server, Client &client, const std::vector<std::string> &args)
{
	server.handleTopic(client, args);
}

void ModeCmd::execute(Server &server, Client &client, const std::vector<std::string> &args)
{
	server.handleMode(client, args);
}

void PassCmd::execute(Server &server, Client &client, const std::vector<std::string> &args)
{
	server.handlePass(client, args);
}

void NickCmd::execute(Server &server, Client &client, const std::vector<std::string> &args)
{
	server.handleNick(client, args);
}

void UserCmd::execute(Server &server, Client &client, const std::vector<std::string> &args)
{
	server.handleUser(client, args);
}