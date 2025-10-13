/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shkaruna <shkaruna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 17:28:05 by spitul            #+#    #+#             */
/*   Updated: 2025/10/13 16:38:08 by shkaruna         ###   ########.fr       */
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
	// server.handleMode(client, data.args);
	server.handleTopic(client, data.args); // TODO remove this
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
qqqq{
	server.handleUser(client, data.args);
}

void WhoCmd::execute(Server &server, Client &client, s_data &data)
{
	server.handleWho(client, data.args);
}
