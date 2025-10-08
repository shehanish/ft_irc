/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:33:31 by lde-taey          #+#    #+#             */
/*   Updated: 2025/10/08 18:45:59 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include <arpa/inet.h>

extern volatile sig_atomic_t signalreceived;

// CONSTRUCTORS

Server::Server() {}

Server::Server(char *port, const std::string& password) : _port(port), _password(password), _adlen(sizeof(_specs))
{
	std::cout << "Server starting... 🟢" << std::endl;
	serverInit();
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
	_channels.clear();
	if (_serverfd >= 0)
		close(_serverfd);
	for (std::map<std::string, Command*>::iterator it = _commands.begin(); 
         it != _commands.end(); ++it)
        delete it->second;
    _commands.clear();
	for (std::map<int, Client*>::iterator cit = _clients.begin(); 
         cit != _clients.end(); ++cit)
	{
        int fd = cit->first;
		close(fd);
		delete cit->second;
	}
	_clients.clear();
	if (_serverfd != -1)
	{
		close(_serverfd);
		_serverfd = -1;
	}
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

std::map<int, Client*>	& Server::getClients()
{
	return (_clients);
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
	for (; mit != _clients.end(); mit ++)
	{
		if (mit->second->getNick() == nick)
			return mit->second;
	}
	return NULL;
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

	int status = getaddrinfo(NULL, _port, &_specs, &_servinfo);
	if (status != 0)
	{
		std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
		throw std::runtime_error("Error: filling serverinfo struct with address infomation failed");
	}
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

// use nc for testing
// example: echo -e "PASS mysecret\r\nNICK anna\r\nUSER anna 0 * :Anna Example\r\n" | nc localhost 6667

/**
 * @brief Parses the extracted message into the components <prefix>, <command> and list of parameters (<params>)..
 *
 * message = [ ":" prefix] SPACE [command] SPACE [args] /r/n
 * There can be a trailing (last argument) that contains spaces, it is preceded by ':'
 * Example ":Angel PRIVMSG Wiz :Hello are you receiving this message ?"
 *
 * @return int Returns true on success, or false when there are errors in the message.
 *
 * @note More detailed error handling still needs to be implemented
 */
bool Server::parse(std::string &msg, Client *client)
{
	s_data data;
	
	int len = msg.size();
	if (len == 0)
		return true; // should be "silently ignored"
	else if (len > 512)
	{
		client->queueMsg(":localhost 417 " + client->getNick() + " :Input too long\r\n");
		return false;
	}

	size_t pos = 0;
	
	// extract prefix
	if (msg[0] == ':')
	{
		size_t space_pos = msg.find(' ');
		if (space_pos == std::string::npos)
			return false; // : without prefix
		data.prefix = msg.substr(1, space_pos - 1);
		std::cout << "The prefix is: " << data.prefix << std::endl; // TODO remove this
		pos = space_pos;
	}
	while (pos < msg.size() && msg[pos] == ' ')
		pos++;
	if (pos >= msg.size())
	{
		std::cerr << "No command found" << std::endl; // TODO should be ignored actually 
		return false;
	}
	
	// extract command
	size_t cmd_start = pos;
    size_t cmd_end = msg.find(' ', cmd_start);
	
	std::string cmd;
    if (cmd_end == std::string::npos)
        cmd = msg.substr(cmd_start);
    else
	{
        cmd = msg.substr(cmd_start, cmd_end - cmd_start);
	}
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper); // convert to capitals
	std::cout << "The command is: " << cmd << std::endl;
	if (cmd_end == std::string::npos)
		pos = msg.size();
	else
		pos = cmd_end;

	// look up command
	std::map<std::string, Command*>::iterator it = _commands.find(cmd);
	if (it == _commands.end())
	{
		std::cerr << "421 ERR_UNKNOWNCOMMAND" << std::endl;
		client->queueMsg(":localhost 421 " + client->getNick() + cmd + " :Unknown command"); // TODO make this work
		return (false);
	}
	pos = cmd_end;
	
	// parse args
	data.args.reserve(10); 
    while (pos < msg.size())	
	{
		while (pos < msg.size() && msg[pos] == ' ')
			pos++;
		if (msg[pos] == ':')
		{
			std::string newarg = msg.substr(pos); // include the ':'
			// problem: According to RFC 2812, the trailing parameter should not include the : itself in the stored argument.
			data.args.push_back(newarg);
			break;
		}
		else
		{
			size_t next_space = msg.find(' ', pos);
			if (next_space == std::string::npos)
            {
                data.args.push_back(msg.substr(pos));
                break;
            }
			std::string nextarg = msg.substr(pos, next_space - pos);
			data.args.push_back(nextarg);
			pos = next_space;
		}
	}
	// TODO remove this (prints args)
	for (size_t i = 0; i < data.args.size(); i++)
	{
		std::cout << "Arg " << i + 1 << ": " << data.args[i] << std::endl;
	}
	
	// pass to execute
	it->second->execute(*this, *client, data);
	return true;
}

void Server::serverInit()
{
	_commands["JOIN"] = new JoinCmd();
	_commands["PART"] = new PartCmd();
	_commands["PRIVMSG"] = new PrivMsgCmd();
	_commands["KICK"] = new KickCmd();
	_commands["INVITE"] = new InviteCmd();
	_commands["TOPIC"] = new TopicCmd();
	_commands["MODE"] = new ModeCmd();
	_commands["PASS"] = new PassCmd();
	_commands["NICK"] = new NickCmd();
	_commands["USER"] = new UserCmd();
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

	while (signalreceived == false)
	{
		if (poll(poll_fds.data(), poll_fds.size(), -1) < 0)
		{
			if (errno == EINTR)
				continue; // interrupted by signal: just retry
			std::cerr << "Poll error: " << std::strerror(errno) << std::endl;
			continue;
		}
		
		for(size_t i = 0; i < poll_fds.size(); i++)
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
					fcntl(client_fd, F_SETFL, O_NONBLOCK); // make client socket non-blocking
					std::cout << "New connection accepted!" << std::endl;

					std::string welcome = "Welcome to our IRC server 🌎!\r\n"; // TODO remove this
					send(client_fd, welcome.c_str(), welcome.length(), 0); // TODO remove this
					pollfd newclient_pollfd = {client_fd, POLLIN | POLLOUT, 0};
					poll_fds.push_back(newclient_pollfd);
					// Cast to sockaddr_in to access sin_addr (choosing IPv4 here!)
					struct sockaddr_in *addr_in = (struct sockaddr_in *)&client_addr;
					std::string client_ip = inet_ntoa(addr_in->sin_addr);
					_clients[client_fd] = new Client(client_fd, client_ip);
				}
				else // existing client sends message
				{
					char data[1000];
					memset(data, 0, sizeof(data));

					int bytesnum = recv(poll_fds[i].fd, data, 1000, 0);
					if (bytesnum > 0)
					{
						std::map<int, Client*>::iterator it_client = _clients.find(poll_fds[i].fd);
						std::vector<std::string> msgs = it_client->second->receiveData(data, bytesnum);
						for (size_t i = 0; i < msgs.size(); i++)
							parse(msgs[i], it_client->second);
					}
					else if (bytesnum == 0) // handles Ctrl-D signal
					{
						std::cout << "Client " << poll_fds[i].fd << " hung up" << std::endl;
						close(poll_fds[i].fd);
						_clients.erase(poll_fds[i].fd);
						poll_fds.erase(poll_fds.begin() + i);
						i--;
					}
					else 
					{
						std::cerr << "Error: " << std::strerror(errno) << std::endl;
						close(poll_fds[i].fd);
						_clients.erase(poll_fds[i].fd);
						poll_fds.erase(poll_fds.begin() + i);
						i--;
					}
				}
			}
			if (poll_fds[i].revents & POLLOUT)
			{
				std::map<int, Client*>::iterator it_client = _clients.find(poll_fds[i].fd);
				if (it_client->second->flush())
				{
					poll_fds[i].events &= ~POLLOUT;
				}
			}
		}
	}
}

void	Server::broadcastMsg(Client &source, Channel *channel, const std::string &msg)
{
	std::set<Client*>::iterator	sit = channel->getMembers().begin();
	for (; sit != channel->getMembers().end(); sit++)
		(*sit)->sendMsg(source, msg); // TODO send msg needs to be adapted to also send prefix 
}

void	Server::handlePass(Client &client, const std::vector<std::string> &args)
{
	if(client.isAuthenticated())
	{
		std::cerr << "462 ERR_ALREADYREGISTRED" << std::endl;
		return; 
	}
	if(args.empty())
	{
		std::cerr << "461 ERR_NEEDMOREPARAMS" <<std::endl;
		return;
	}
	if(args[0] != _password)
	{
		std::cerr << "464 ERR_PASSWDMISMATCH" << std::endl;
		return;
	}
	client.setIsAuthenticated(true);
}

bool	Server::isNickTaken(const std::string &nickname) const
{
	for(std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNick() == nickname)
			return true;
	}
	return false;
}

void	Server::handleNick(Client &client, const std::vector <std::string> &args)
{
	if(args.empty())
	{
		std::cerr << "Empty arguments!" << std::endl;
		return;
	}
	std::string nickname = args[0];
	
	if(isNickTaken(nickname))
	{
		std::cerr << "Nick name has already taken!" << std::endl;
		return;
	}
	client.setNick(nickname);
	std::cout << "Nickname set to  " << nickname << std::endl;
}

void Server::registerClient(Client &client)
{
    if (client.isRegistered())
        return;

    // Needs ALL 3: authenticated + nick + username
    if (!client.isAuthenticated() || client.getNick().empty() || client.getUserName().empty())
        return;

    client.setRegistered(true);

    // Send welcome numerics (later)
    std::cout << "Client registered: " 
              << client.getNick() << "!" << client.getUserName() << std::endl;
}


void Server::handleUser(Client &client, const std::vector<std::string> &args)
{
    if (args.size() < 4)
    {
        std::cerr << "461 ERR_NEEDMOREPARAMS (USER)" << std::endl;
        return;
    }

    if (!client.getUserName().empty())
    {
        std::cerr << "462 ERR_ALREADYREGISTRED (USER)" << std::endl;
        return;
    }

    std::string username = args[0];
    std::string realname = args[3]; // assuming it's parsed correctly

    client.setUserName(username);
    client.setRealName(realname);

    std::cout << "User set: " << username << " (" << realname << ")" << std::endl;

    // Try to complete registration (PASS + NICK + USER must be done)
    registerClient(client);
}

/*
PASS must succeed → client.setIsAuthenticated(true).

NICK must succeed → client.setNick(nickname).

USER must succeed → client.setUserName(...), client.setRealName(...).

After each of NICK or USER, call registerClient(client).
*/

