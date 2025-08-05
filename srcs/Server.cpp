/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:33:31 by lde-taey          #+#    #+#             */
/*   Updated: 2025/08/05 19:14:11 by lde-taey         ###   ########.fr       */
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
		this->_port = other._port;
		this->_password = other._password;
		this->_serverfd = other._serverfd;
		this->_address = other._address;
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


/**
 * @brief Initializes and configures the server socket for IPv4 TCP connections.
 *
 * This function creates a socket, sets up the address structure, binds the socket
 * to the specified port and address (so the kernel knows where to send packages), 
 * and puts the socket into listening mode to accept incoming client connections.
 *
 * @return int Returns 0 on success, or 1 if an error occurs during socket creation,
 * 		binding, or listening.
 *
 * @note Error handling is performed for socket creation, bind, and listen failures.
 *       On error, an error message is printed and the socket is closed if necessary.
 */

int Server::setUpSocket()
{
	_serverfd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET is for IPv4, SOCK_STREAM is for tcp/ip 
	if(_serverfd < 0)
	{
		std::cerr << "Error: Could not create socket" << std::endl;
		return 1; // or exit?
	}
	
	// add more specifications here
	
	memset(&_address, 0, sizeof(_address)); // initialize the address struct to zeroes
	_address.sin_family = AF_INET; // ipv4
	_address.sin_addr.s_addr = INADDR_ANY; // any address can connect
	_address.sin_port = htons(_port); // convert host to network notation (necessary check)
	
	if (bind(_serverfd, (struct sockaddr*)&_address, sizeof(_address)) < 0)
	{
		std::cerr << "Error: Could not bind socket" << std::endl;
		close (_serverfd);
		return 1; // or exit?
	}
	
	if (listen(_serverfd, SOMAXCONN) < 0)
	{
		std::cerr << "Error: Could not put socket in listening mode" << std::endl;
		close (_serverfd);
		return 1; // or exit?
	}

	std::cout << "Server ready and listening on port " << _port << std::endl;
	
	return (0);
}


void Server::run()
{
	// work in progress
}