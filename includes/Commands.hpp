/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shkaruna <shkaruna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 07:38:28 by spitul            #+#    #+#             */
/*   Updated: 2025/10/13 16:37:28 by shkaruna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <vector>
#include <string>

class Server;
class Client;
struct s_data;

class Command
{
	public:
		Command() {}
		virtual void execute(Server &server, Client &client, s_data &data) = 0;
		virtual ~Command() {}
};

class JoinCmd	: public Command
{
	void execute(Server &server, Client &client, s_data &data);
};

class PartCmd	: public Command
{
	void execute(Server &server, Client &client, s_data &data);
};

class PrivMsgCmd	: public Command
{
	void execute(Server &server, Client &client, s_data &data);
};

class KickCmd	: public Command
{
	void execute(Server &server, Client &client, s_data &data);
};

class InviteCmd	: public Command
{
	void execute(Server &server, Client &client, s_data &data);
};

class TopicCmd	: public Command
{
	void execute(Server &server, Client &client, s_data &data);
};

class ModeCmd	: public Command
{
	void execute(Server &server, Client &client, s_data &data);
};

class PassCmd	: public Command
{
	void execute(Server &server, Client &client, s_data &data);
};

class NickCmd	: public Command
{
	void execute(Server &server, Client &client, s_data &data);
};

class UserCmd	: public Command
{
	void execute(Server &server, Client &client, s_data &data);
};

class WhoCmd	: public Command
{
	void execute(Server &server, Client &client, s_data &data);
};

#endif
