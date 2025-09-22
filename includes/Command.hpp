/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 07:38:28 by spitul            #+#    #+#             */
/*   Updated: 2025/09/22 11:52:53 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vector>
#include <string>

class Server;
class Client;

class Command
{
	public:
		Command() {}
		virtual void execute(Server &server, Client &client, const std::vector<std::string> &args) = 0;
		virtual ~Command() {}
};

class JoinCmd	: public Command
{
	void execute(Server &server, Client &client, const std::vector<std::string> &args);
};

class PartCmd	: public Command
{
	void execute(Server &server, Client &client, const std::vector<std::string> &args);
};

class PrivMsgCmd	: public Command
{
	void execute(Server &server, Client &client, const std::vector<std::string> &args);
};

class KickCmd	: public Command
{
	void execute(Server &server, Client &client, const std::vector<std::string> &args);
};

class InviteCmd	: public Command
{
	void execute(Server &server, Client &client, const std::vector<std::string> &args);
};

class TopicCmd	: public Command
{
	void execute(Server &server, Client &client, const std::vector<std::string> &args);
};

class ModeCmd	: public Command
{
	void execute(Server &server, Client &client, const std::vector<std::string> &args);
};

#endif
