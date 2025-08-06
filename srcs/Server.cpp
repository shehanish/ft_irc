/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:33:31 by lde-taey          #+#    #+#             */
/*   Updated: 2025/08/06 17:49:03 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

// CONSTRUCTORS

Server::Server()
{
	// std::cout << "Server Default constructor called" << std::endl;
}

Server::Server(char *port, const std::string& password) : _port(port), _password(password), _adlen(sizeof(_specs))
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
		this->_specs = other._specs;
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
 * This function creates a server socket, sets socket options, sets up the specs structure,
 * binds the socket to the specified port using the address info, and puts the socket into listening mode.
 *
 * @return int Returns 0 on success, or throws std::runtime_error on failure.
 *
 * @note Error handling is performed for socket creation, bind, and listen failures.
 *       On error, an exception is thrown and the socket is closed if necessary.
 */

int Server::setUpSocket()
{
	memset(&_specs, 0, sizeof(_specs)); // initialize the specifications struct to zeroes
	_specs.ai_family = AF_UNSPEC; // ipv4 or ipv6
	_specs.ai_socktype = SOCK_STREAM; // TCP stream sockets
	_specs.ai_flags = AI_PASSIVE; // fill in my IP for me
	_adlen = sizeof(_specs);

	if (getaddrinfo(NULL, _port, &_specs, &_servinfo) != 0)
		throw std::runtime_error("Error: filling serverinfo struct with address infomation failed");

	_serverfd = socket(_servinfo->ai_family, _servinfo->ai_socktype, _servinfo->ai_protocol); 
	if(_serverfd < 0)
		throw std::runtime_error("Error: Could not create socket");
	
	int yes = 1;
	if (setsockopt(_serverfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		throw std::runtime_error("Error: Server cannot be reused");
		
	if (bind(_serverfd, _servinfo->ai_addr, _servinfo->ai_addrlen) < 0)
		throw std::runtime_error("Error: Bind failed");
	
	if (listen(_serverfd, SOMAXCONN) < 0) // SOMAXCONN means that the max of 128 incoming connections can be queued
		throw std::runtime_error("Error: Could not put socket in listening mode");

	std::cout << "Server ready and listening on port " << _port << std::endl;

	freeaddrinfo(_servinfo);
	return (0);
}

/**
 * @brief Main server loop that accepts and handles incoming client connections.
 *
 * This function continuously waits for new client connections using accept().
 * For each accepted connection, it sends a welcome message to the client and then closes the connection.
 * If accept() fails, an error message is printed and the loop continues.
 *
 * @note This implementation handles one client at a time and closes the connection immediately after sending the welcome message.
 *       For a production IRC server, concurrent client handling and more robust communication is needed. Client fd and address
 * 		information should also be linked to the Client class in this program.
 */

void Server::loop()
{
	std::vector<pollfd>	poll_fds;
	pollfd				server_poll_fd = {_serverfd, POLLIN, 0};
	sockaddr_storage	client_addr;
	socklen_t			client_addrlen = sizeof(client_addr);

	poll_fds.push_back(server_poll_fd); // add server socket to poll list
	
	while (1)
	{
		if (poll(poll_fds.data(), poll_fds.size(), -1) < 0)
		{
			std::cerr << "Poll error: " << std::strerror(errno) << std::endl; // check if errno works here
			continue;
		}
		
		for(size_t i = 0; i < poll_fds.size(); i++)
		{
			if (poll_fds[i].revents & POLLIN)
			{
				if (poll_fds[i].fd == _serverfd)
				{
					int client_fd = accept(_serverfd, (struct sockaddr *)&client_addr, &client_addrlen);
					if (client_fd < 0)
					{
						std::cerr << "Failed to accept new client: " << std::strerror(errno) << std::endl;
						continue;
					}
					std::cout << "New connection accepted!" << std::endl;

					std::string welcome = "Welcome to our IRC server 🌎!\r\n";
					send(client_fd, welcome.c_str(), welcome.length(), 0);
					close(client_fd);
				}
				else
				{
					std::cout << "Work in progress" << std::endl;
				}
			}
		}
		break; // remove this
	}
}