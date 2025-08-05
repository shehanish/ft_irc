/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:33:31 by lde-taey          #+#    #+#             */
/*   Updated: 2025/08/05 18:53:03 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

// CONSTRUCTORS

Server::Server()
{
	// std::cout << "Server Default constructor called" << std::endl;
}

Server::Server(int port, const std::string& password) : _port(port), _password(password)
{
	// std::cout << "Server constructor called" << std::endl;
	setUpSocket();
}

Server::Server(const Server& other)
{
	// std::cout << "Server Copy constructor called" << std::endl;
	*this = other;
}

Server& Server::operator=(const Server& other)
{
	// std::cout << "Server Copy Assignment constructor called" << std::endl;
	if (this != &other) 
	{
		// copy fields here
	}
	return *this;
}

// DESTRUCTOR

Server::~Server() 
{
	// std::cout << "Server Destructor called" << std::endl;
}

// MEMBER FUNCTIONS

// this function uses the low level sockaddr struct (which only works for ipv4)
// but we can upgrade it for ipv6 functionalities by using getaddrinfo() later

int Server::setUpSocket()
{
	// creates a server socket and gives it an fd
	// AF_INET is for IPv4, SOCK_STREAM is for tcp/ip 
	_serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if(_serverfd < 0)
	{
		// std::cerr << "Error: Could not create socket" << std::endl;
		strerror(errno);
		return 1; // or exit?
	}
	
	// add more specifications here
	
	// prepares the socket address
	memset(&_address, 0, sizeof(_address)); // initialize the address struct to zeroes
	_address.sin_family = AF_INET; // ipv4
	_address.sin_addr.s_addr = INADDR_ANY; // any address can connect
	_address.sin_port = htons(_port); // convert host to network notation (necessary check)
	
	// binds the server socket to the address, so the kernel knows where to send packages
	bind(_serverfd, (struct sockaddr*)&_address, sizeof(_address));
	
	// puts the server in listening mode (ready for connections)
	listen(_serverfd, SOMAXCONN);

	std::cout << "Server ready and listening on port " << _port << std::endl;
	
	return (0);
}


void Server::run()
{
	// work in progress
}