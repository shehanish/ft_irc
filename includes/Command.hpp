/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shkaruna <shkaruna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 16:55:21 by shkaruna          #+#    #+#             */
/*   Updated: 2025/09/16 17:49:12 by shkaruna         ###   ########.fr       */
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
		Command();
		Command(const Command& oth);
		Command& operator=(const Command& oth);
		~Command();
};

#endif