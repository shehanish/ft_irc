/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:33:31 by lde-taey          #+#    #+#             */
/*   Updated: 2025/08/06 13:05:08 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

// CONSTRUCTORS

Server::Server()
{
	// std::cout << "Server Default constructor called" << std::endl;
}

Server::Server(int port, const std::string& password) : _port(port), _password(password), _adlen(sizeof(_address))
{
	std::cout << "Server starting... 🟢" << std::endl;
	setUpSocket();
}

Server::Server(const Server& other)
{
	*this = other;
}

Server& Server::operator=(const Server& other)
{
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
	std::cout << "Server shutting down 🔴" << std::endl;
	if (_serverfd >= 0)
		close(_serverfd);
}

// MEMBER FUNCTIONS


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
		throw std::runtime_error("Error: Could not create socket");
	
	int optvalue = 1;
	if (setsockopt(_serverfd, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(optvalue)) == 1)
		throw std::runtime_error("Error: Server cannot be reused");
		
	// we use the low level sockaddr_in struct here (which only works for ipv4)
	// but we can upgrade later for ipv6 functionalities by using getaddrinfo()
	memset(&_address, 0, sizeof(_address)); // initialize the address struct to zeroes
	_address.sin_family = AF_INET; // ipv4
	_address.sin_addr.s_addr = INADDR_ANY; // any address can connect
	_address.sin_port = htons(_port); // convert host to network notation (necessary check)
	
	if (bind(_serverfd, (struct sockaddr*)&_address, sizeof(_address)) < 0)
		throw std::runtime_error("Error: Bind failed");
	
	if (listen(_serverfd, SOMAXCONN) < 0) // SOMAXCONN means that 128 incoming connections can be queued
		throw std::runtime_error("Error: Could not put socket in listening mode");

	std::cout << "Server ready and listening on port " << _port << std::endl;
	
	return (0);
}


void Server::run()
{
	int client_fd;
	
	while (1)
	{
		client_fd = accept(_serverfd, (struct sockaddr *)&_address, &_adlen);
		if (client_fd < 0)
		{
			std::cerr << "Accept failed: " << std::strerror(errno) << std::endl;
			usleep(1000);
			continue;
		}
		std::cout << "New connection accepted!" << std::endl;

		std::string welcome = "Welcome to our IRC server!\r\n";
		send(client_fd, welcome.c_str(), welcome.length(), 0);
		close(client_fd);
	}
}