/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:33:31 by lde-taey          #+#    #+#             */
/*   Updated: 2025/10/27 15:37:26 by lde-taey         ###   ########.fr       */
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
	std::cout << "Waiting for the client.... " << std::endl;
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
		// std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
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

	// std::cout << "Server ready and listening on port " << _port << std::endl;

	freeaddrinfo(_servinfo);
	return (0);
}

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
        return true; // silently ignore empty messages
    else if (len > 512)
        return false; // max IRC message length

    // Trim \r\n
    size_t end = msg.find_last_not_of("\r\n");
    if (end == std::string::npos)
        return true; // only \r\n
    msg = msg.substr(0, end + 1);

    size_t pos = 0;

    // Extract prefix if present
    if (msg[0] == ':')
    {
        size_t space_pos = msg.find(' ');
        if (space_pos == std::string::npos)
            return false; // invalid
        data.prefix = msg.substr(1, space_pos - 1);
        pos = space_pos + 1;
    }

    while (pos < msg.size() && msg[pos] == ' ')
        pos++;

    if (pos >= msg.size())
        return false; // no command

    // Extract command
    size_t cmd_end = msg.find(' ', pos);
    std::string cmd = (cmd_end == std::string::npos) ? msg.substr(pos) : msg.substr(pos, cmd_end - pos);
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    pos = (cmd_end == std::string::npos) ? msg.size() : cmd_end + 1;

    // Parse arguments
    std::vector<std::string> args;
    while (pos < msg.size())
    {
        while (pos < msg.size() && msg[pos] == ' ')
            pos++;
        if (pos >= msg.size()) break;

        if (msg[pos] == ':')
        {
            args.push_back(msg.substr(pos + 1)); // skip ':'
            break;
        }
        size_t next_space = msg.find(' ', pos);
        if (next_space == std::string::npos)
        {
            args.push_back(msg.substr(pos));
            break;
        }
        args.push_back(msg.substr(pos, next_space - pos));
        pos = next_space + 1;
    }

	if (cmd == "PASS")
	{
		handlePass(*client, args);
	}
	else if (cmd == "NICK")
	{
		handleNick(*client, args);
	}
	else if (cmd == "USER")
	{
		handleUser(*client, args);
	}
	else if (cmd == "CAP")
    {
        handleCap(*client, args);
    }
	else
    {
        // For all other commands, check if client is registered
        if (!client->isRegistered())
        {
            std::string error = ":localhost 451 * :You have not registered\r\n";
            client->appendToSendBuffer(error);
            return true;
        }
        
        // Route through command pattern
        std::map<std::string, Command*>::iterator it = _commands.find(cmd);
        if (it != _commands.end())
        {
            data.args = args;
            it->second->execute(*this, *client, data);
        }
        else
        {
            std::string error = ":localhost 421 " + client->getNick() + " " + cmd + " :Unknown command\r\n";
            client->appendToSendBuffer(error);
        }
    }

    return true;
}

void Server::handleCap(Client &client, const std::vector<std::string> &args)
{
    if (args.empty())
        return;

    std::string subCmd = args[0];

    if (subCmd == "LS")
    {
        // Reply with capability list (empty for now - no capabilities supported)
        // HexChat sends "CAP LS 302" so it might include version number in args[1]
        std::string reply = ":localhost CAP * LS :\r\n";
        client.appendToSendBuffer(reply);
		// std::cout << "Handled CAP LS for client " << client.getFd() << std::endl;
    }
    else if (subCmd == "END")
    {
        // CAP negotiation ended, client is ready to proceed with registration
		// std::cout << "Client " << client.getFd() << " ended CAP negotiation." << std::endl;
        // Try to complete registration in case PASS/NICK/USER were already sent
        registerClient(client);
    }
    else if (subCmd == "REQ")
    {
        // Client requesting capabilities - we don't support any yet
        if (args.size() > 1)
        {
            std::string reply = ":localhost CAP * NAK :" + args[1] + "\r\n";
            client.appendToSendBuffer(reply);
        }
    }
    else
    {
        // Ignore other CAP subcommands silently
		// std::cout << "Ignoring CAP " << subCmd << " from client " << client.getFd() << std::endl;
    }
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
	_commands["WHO"] = new WhoCmd();
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

					pollfd newclient_pollfd = {client_fd, POLLIN | POLLOUT, 0};
					poll_fds.push_back(newclient_pollfd);
					// Cast to sockaddr_in to access sin_addr (choosing IPv4 here!)
					struct sockaddr_in *addr_in = (struct sockaddr_in *)&client_addr;
					std::string client_ip = inet_ntoa(addr_in->sin_addr);
					_clients[client_fd] = new Client(client_fd, client_ip);
					std::string welcome = "Welcome to our IRC server 🌎!\r\n";
					_clients[client_fd]->appendToSendBuffer(welcome);
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
						for (size_t j = 0; j < msgs.size(); j++)
							parse(msgs[j], it_client->second);
						
						// After parsing commands, check ALL clients for pending data to send
						// This is important for broadcasts (PRIVMSG to channels, JOIN notifications, etc.)
						for (size_t k = 1; k < poll_fds.size(); k++)
						{
							std::map<int, Client*>::iterator check_client = _clients.find(poll_fds[k].fd);
							if (check_client != _clients.end() && !check_client->second->getSendBuffer().empty())
							{
								// Only enable POLLOUT if not already enabled
								if (!(poll_fds[k].events & POLLOUT))
									poll_fds[k].events |= POLLOUT;
							}
						}
					}
					else if (bytesnum == 0)
					{
						std::cout << "Client " << poll_fds[i].fd << " hung up" << std::endl;
						removeFromChannels(poll_fds[i].fd);
						_clients.erase(poll_fds[i].fd);
						poll_fds.erase(poll_fds.begin() + i);
						close(poll_fds[i].fd);
						i--;
					}
					else 
					{
						std::cerr << "Error: " << std::strerror(errno) << std::endl;
						removeFromChannels(poll_fds[i].fd);
						_clients.erase(poll_fds[i].fd);
						poll_fds.erase(poll_fds.begin() + i);
						close(poll_fds[i].fd);
						i--;
					}
				}
			}
			if (poll_fds[i].revents & POLLOUT)
			{
				std::map<int, Client*>::iterator it_client = _clients.find(poll_fds[i].fd);
				if (it_client != _clients.end() && it_client->second->flush())
				{
					// Buffer is empty, disable POLLOUT
					poll_fds[i].events &= ~POLLOUT;
				}
			}
		}
	}
}

void	Server::removeFromChannels(int client_fd)
{
    // Get client object
    Client* client = _clients[client_fd];

    // Iterate over all channels
    std::string quitMsg = ":" + client->getNick() + "!" + client->getUserName() + "@host QUIT :Client disconnected\r\n";
	for (std::map<std::string, Channel*>::iterator ch_it = _channels.begin(); ch_it != _channels.end(); ++ch_it)    
	{
        Channel* channel = ch_it->second;
        // Remove client from channel
        channel->delUser(*client);

        // Broadcast quit message to remaining members
		std::set<Client*>::iterator it = channel->getMembers().begin();
		for (; it != channel->getMembers().end(); it++)
		{
			(*it)->appendToSendBuffer(quitMsg);
		}
		if (channel->empty())
    	{
        	delete channel;
        	_channels.erase(ch_it++);
    	}
    }
}

void	Server::handlePass(Client &client, const std::vector<std::string> &args)
{
	// If already authenticated, ignore subsequent PASS commands
	if (client.isAuthenticated())
	{
		// 462 - ERR_ALREADYREGISTERED;
		std::string error = ":localhost 462 " + client.getNick() + " :You may not reregister\r\n";
		client.appendToSendBuffer(error);
		return;	
	}
	
	if(args.empty())
	{
		std::string error = ":localhost 461 * PASS :Not enough parameters\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	if(args[0] != _password)
	{
		std::string error = ":localhost 464 * :Password incorrect\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	client.setIsAuthenticated(true);
	
	// Try to complete registration if NICK and USER were already sent
	// (This handles cases where HexChat sends NICK/USER before PASS)
	registerClient(client);
}

bool	Server::isNickTaken(const std::string &nickname, const Client *excludeClient) const
{
	for(std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		// Skip the client that's trying to change their nickname
		if (excludeClient && it->second == excludeClient)
			continue;
		if (it->second->getNick() == nickname)
			return true;
	}
	return false;
}

void	Server::handleNick(Client &client, const std::vector <std::string> &args)
{
	if(args.empty())
	{
		std::string error = ":localhost 431 * :No nickname given\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	std::string newNickname = args[0];
	std::string oldNickname = client.getNick();
	
	// Check if new nickname is already taken (excluding this client)
	if(isNickTaken(newNickname, &client))
	{
		std::string error = ":localhost 433 * " + newNickname + " :Nickname is already in use\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	if (newNickname[0] == '#')
	{
		
		std::string error = ":localhost 432 * " + newNickname + " :Erroneous nickname\r\n";
		client.appendToSendBuffer(error);
		return;
	}
	
	// If client is already registered, send NICK change notification
	if (client.isRegistered() && !oldNickname.empty())
	{
		// Notify the client about their nickname change
		std::string nickMsg = ":" + oldNickname + "!~" + client.getUserName() + "@localhost NICK :" + newNickname + "\r\n";
		client.appendToSendBuffer(nickMsg);
		std::cout << "Nickname changed: " << oldNickname << " → " << newNickname << std::endl;
	}
	else
	{
		std::cout << "Nickname set to " << newNickname << std::endl;
	}
	
	client.setNick(newNickname);
	
	// Try to complete registration after setting nickname (for new connections)
	// Registration will only succeed if PASS was already sent
	if (!client.isRegistered())
		registerClient(client);
}

void Server::sendWelcome(Client &client)
{
    std::string nick = client.getNick();
    std::string welcome = ":ircserv 001 " + nick + " :Welcome to the IRC server " + nick + "\r\n";
    client.appendToSendBuffer(welcome);

    std::string host = ":ircserv 002 " + nick + " :Your host is ircserv, running version 1.0\r\n";
    client.appendToSendBuffer(host);

    std::string created = ":ircserv 003 " + nick + " :This server was created today\r\n";
    client.appendToSendBuffer(created);

    std::string info = ":ircserv 004 " + nick + " ircserv 1.0 o o\r\n";
    client.appendToSendBuffer(info);
}

void Server::registerClient(Client &client)
{
    if (client.isRegistered())
	{
		return;
	}
        
    // Needs ALL 3: authenticated + nick + username
    if (!client.isAuthenticated() || client.getNick().empty() || client.getUserName().empty())
	{
		// Not ready for registration yet
		return;
	}
        
    client.setRegistered(true);

    // Send welcome numerics
    std::cout << "Client registered: " 
              << client.getNick() << "!" 
			  << client.getUserName() << std::endl;
	sendWelcome(client); // send IRC numeric messages 001-004
}
void Server::handleUser(Client &client, const std::vector<std::string> &args)
{
    if (args.size() < 4)
    {
        std::string error = ":localhost 461 * USER :Not enough parameters\r\n";
        client.appendToSendBuffer(error);
        return;
    }

    std::string username = args[0];
    std::string realname = args[3];
    
    // If client is already registered, allow username change
    if (client.isRegistered())
    {
        std::string oldUsername = client.getUserName();
        client.setUserName(username);
        client.setRealName(realname);
        
        // Notify the client about username change
        std::string msg = ":localhost NOTICE " + client.getNick() + " :Username changed from " + oldUsername + " to " + username + "\r\n";
        client.appendToSendBuffer(msg);
        std::cout << "Username changed: " << oldUsername << " → " << username << std::endl;
        return;
    }

    // For new clients: set username and try to complete registration
    client.setUserName(username);
    client.setRealName(realname);

    std::cout << "User set: " << username << " (" << realname << ")" << std::endl;

    // Try to complete registration (PASS + NICK + USER must all be done)
    // This will silently fail if PASS hasn't been sent yet
    registerClient(client);
}

