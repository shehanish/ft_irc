/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 17:28:05 by spitul            #+#    #+#             */
/*   Updated: 2025/10/08 17:08:56 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void JoinCmd::execute(Server &server, Client &client, s_data &data)
{
	server.handleJoin(client, data.args);
}

void PartCmd::execute(Server &server, Client &client, s_data &data)
{
	server.handlePart(client, data.args);
}

void PrivMsgCmd::execute(Server &server, Client &client, s_data &data)
{
	server.handlePrivMsg(client, data.args);
}

void KickCmd::execute(Server &server, Client &client, s_data &data)
{
	server.handleKick(client, data.args);
}

void InviteCmd::execute(Server &server, Client &client, s_data &data)
{
	server.handleInvite(client, data.args);
}

void TopicCmd::execute(Server &server, Client &client, s_data &data)
{
	server.handleTopic(client, data.args);
}

void ModeCmd::execute(Server &server, Client &client, s_data &data)
{
	server.handleMode(client, data.args);
}

void PassCmd::execute(Server &server, Client &client, s_data &data)
{
	server.handlePass(client, data.args);
}

void NickCmd::execute(Server &server, Client &client, s_data &data)
{
	server.handleNick(client, data.args);
}

void UserCmd::execute(Server &server, Client &client, s_data &data)
{
	server.handleUser(client, data.args);
}