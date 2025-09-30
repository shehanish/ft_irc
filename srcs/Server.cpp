/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:33:31 by lde-taey          #+#    #+#             */
/*   Updated: 2025/09/30 07:08:22 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"


// CONSTRUCTORS

Server::Server()
{
	// std::cout << "Server Default constructor called" << std::endl;
	_commands["JOIN"] = new JoinCmd();
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
	for (std::map<std::string, Channel*>::iterator mit = _channels.begin(); mit != _channels.end(); mit++)
		delete mit->second;
	if (_serverfd >= 0)
		close(_serverfd);
}

// GETTERS AND SETTERS

char* Server::getPort() const
{
	return (_port);
}

std::string Server::getPassword() const
{
	return (_password);
}

int Server::getServerfd() const
{
	return (_serverfd);
}

std::map<int, Client>	& Server::getClients()
{
	return (_clients);
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
 * @brief Main server loop that accepts and handles incoming client connections using poll().
 *
 * This function uses poll() to monitor the server socket and all connected client sockets for activity.
 * When a new client connects, it accepts the connection, sends a welcome message, and adds the client to the poll list.
 * For existing clients, it receives messages and handles client disconnections or errors.
 *
 * @note In a next step, this function can be refactored into smaller functions. 
 */

void Server::loop()
{
	std::vector<pollfd>	poll_fds; // list of sockets we're watching for activity
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
		
		for(size_t i = 0; i < poll_fds.size(); i++) // use iterator instead?
		{
			if (poll_fds[i].revents & POLLIN) // new client connection
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
					pollfd newclient_pollfd = {client_fd, POLLIN | POLLOUT, 0};
					poll_fds.push_back(newclient_pollfd);
					// add to client struct as well
				}
				else // existing client sends message
				{
					char message[4096]; // check irc documentation
					memset(message, 0, sizeof(message));

					int bytesnum = recv(poll_fds[i].fd, message, 4096, 0);
					if (bytesnum > 0)
					{
						std::string msg(message, bytesnum);
						std::cout << "Received from client: " << msg;
						// here parsing needs to happen
					}
					else if (bytesnum == 0)
					{
						std::cout << "Client " << poll_fds[i].fd << " hung up" << std::endl;
						close(poll_fds[i].fd);
						poll_fds.erase(poll_fds.begin() + i);
						i--;
					}
					else 
					{
						std::cerr << "Error: " << std::strerror(errno) << std::endl;
						close(poll_fds[i].fd);
						poll_fds.erase(poll_fds.begin() + i);
						i--;
					}
				}
			}
		}
	}
}

Channel*	Server::getChannel(const std::string &channel)
{
	std::map<std::string, Channel*>::iterator	mit = _channels.find(channel);
		if (mit != _channels.end())
			return mit->second;
	return NULL;
}

Channel*	Server::createChannel(const std::string &channel, Client &creator)
{
	if (getChannel(channel) != NULL)
		return getChannel(channel);
		
	Channel	*newChannel = new Channel(channel, &creator);
	_channels[channel] = newChannel;
	return newChannel;
}

Client*	Server::getUser(const std::string &nick)
{
	std::map<int, Client*>::iterator	mit = _clients.begin();
	for (mit; mit != _clients.end(); mit ++)
	{
		if (mit->second->getName() == nick)
			return mit->second;
	}
	return NULL;
}

void	Server::broadcastMsg(Client &client, std::vector<std::string> args)
{
	std::string	&msg = client.getMsg(args);
	std::vector<std::string>::iterator	it = args.begin();
	while (it != args.end())
	{
		if ((*it)[0] == '#')
		{
			Channel	*channel = getChannel(*it);
			if (channel == NULL)
				{}//401 ERR_NOSUCHNICK "<nickname> :No such nick/channel"
			std::set<Client*>::iterator	sit = channel->getMembers().begin();
			for (sit; sit != channel->getMembers().end(); sit++)
				client.sendMsg(**sit, msg);
		}
		else
		{
			Client	*target = getUser(*it);
			if (target == NULL)
				{} //401 ERR_NOSUCHNICK "<nickname> :No such nick/channel"
			client.sendMsg(*target, msg);
		}
	}
}

void	Server::handleJoin(Client &client, const std::vector<std::string> &args)
{
	Channel	*channel;
	
	if (args.empty())
		return; //message
	if (client.getNbChannel() > MAX_CHANNELS)
		return; //send message
	channel = getChannel(args[0]);
	if (channel == NULL)
	{
		channel = createChannel(args[0], client);
	}
	else
	{
		if (channel->hasKey() && !channel->checkKey(args[1]))
			return;
		if (!channel->isInvited(client))
			return;
		channel->addUser(client);
		client.addChannel();
		//send messages + topic
	}
}

void	Server::handlePart(Client &client, const std::vector<std::string> &args)
{
	if (args.empty())
		return;
	Channel	*channel;
	std::vector<std::string>::const_iterator	it = args.begin();
	for (it; it != args.end(); it ++)
	{
		channel = getChannel(*it);
		if (!channel->isMember(client))
		{
			//send msg
			continue;
		}
		channel->delUser(client);
		if (channel->isOperator(client))
			channel->delOperator(client);
		if (channel->isInviteOnly())
			channel->delInvitation(client);
		//send part message
	}
		
}

void	Server::handlePrivMsg(Client &client, const std::vector<std::string> &args)
{
	if (!args.empty())
		broadcastMsg(client, args);
}

// KICK <channel> <user> [<comment>]

void	Server::handleKick(Client &client, const std::vector<std::string> &args)
{
	
	

}

void	Server::handleInvite(Client &client, const std::vector<std::string> &args)
{
	
}

void	Server::handleTopic(Client &client, const std::vector<std::string> &args);
void	Server::handleMode(Client &client, const std::vector<std::string> &args);
