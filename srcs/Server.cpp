/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:33:31 by lde-taey          #+#    #+#             */
/*   Updated: 2025/08/01 17:35:42 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

// CONSTRUCTORS

Server::Server()
{
	std::cout << "Server Default constructor called" << std::endl;
}

Server::Server(int port, const std::string& password) : _port(port), _password(password)
{
	std::cout << "Server constructor called" << std::endl;
}

Server::Server(const Server& other)
{
	std::cout << "Server Copy constructor called" << std::endl;
	*this = other;
}

Server& Server::operator=(const Server& other)
{
	std::cout << "Server Copy Assignment constructor called" << std::endl;
	if (this != &other) 
	{
		// copy fields here
	}
	return *this;
}

// DESTRUCTOR

Server::~Server() 
{
	std::cout << "Server Destructor called" << std::endl;
}

// MEMBER FUNCTIONS

void Server::run()
{
	
}