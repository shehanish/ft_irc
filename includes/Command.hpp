/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shkaruna <shkaruna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 16:55:21 by shkaruna          #+#    #+#             */
/*   Updated: 2025/09/18 15:55:38 by shkaruna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <string>
#include <vector>

class Client; // Forward declaration 
class Server;

class Command
{
	private:
		std::string raw_message;
		std::string	command_name;
		std::vector<std::string> arguments;
		Client*			client;
		Server*		server;
		std::vector<std::string> response;
		
	public:
		Command();
		Command(const std::string& raw_message, Client* client, Server* server);
		Command(const Command& oth);
		Command& operator=(const Command& oth);
		~Command();

		//getters
		std::string getRawMessage() const;
		std::string getCommandName() const;
		const std::vector<std::string>& getArguments() const;
		Client* getClient() const;
		const std::vector<std::string>& getResponse() const;

		void	parse_message(const std::string& raw_message);
};

#endif