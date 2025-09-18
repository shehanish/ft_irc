/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shkaruna <shkaruna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 16:54:55 by shkaruna          #+#    #+#             */
/*   Updated: 2025/09/18 15:55:52 by shkaruna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

Command::Command() 
	: raw_message(""), command_name(""), arguments(), client(NULL), server(NULL), 
	response() {};

Command::Command(const std::string& raw_message, Client* client, 
	Server* server) 
	:raw_message(raw_message),
	 client(client),
	 server(server),
	 arguments(),
	 response() {};

Command::Command(const Command& oth)
	:raw_message(oth.raw_message),
	 command_name(oth.command_name), 
	 arguments(oth.arguments),
	 client(oth.client),
	 server(oth.server),
	 response(oth.response) {};

Command& Command::operator=(const Command& oth)
{
	if(this != &oth)
	{
		raw_message = oth.raw_message;
		command_name = oth.command_name;
		arguments = oth.arguments;
		client = oth.client;
		server = oth.server;
		response = oth.response;
	}
	return *this;
}
	

Command::~Command() {};

//getters
std::string Command::getRawMessage() const
{
	return raw_message;
}

std::string Command::getCommandName() const
{
	return command_name;
}

const std::vector<std::string>& Command::getArguments() const
{
	return arguments;
}

Client* Command::getClient() const
{
	return client;
}

const std::vector<std::string>& Command::getResponse() const
{
	return response;
}

void	Command::parse_message(const std::string& raw)
{
	raw_message = raw;

	//removing /n and /r
	std::string line = raw_message;
	if(!line.empty() && line[line.size() - 1] == '\n')
		line.erase(line.size() - 1);
	if(!line.empty() && line[line.size() - 1] == '\r')
		line.erase(line.size() - 1);
		
	for(int i = 0; i < raw[i]; ++i)
	{
		if(line.size() - 1 == ' ')
			
			
		
	}
}


