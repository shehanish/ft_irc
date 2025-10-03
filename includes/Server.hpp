/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:25:25 by lde-taey          #+#    #+#             */
/*   Updated: 2025/10/03 19:40:46 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Channel.hpp"
# include "Client.hpp"
# include "Commands.hpp"
# include "Declarations.hpp"
# include <algorithm>
# include <cstring>
# include <errno.h>
# include <iostream>
# include <map>
# include <netdb.h>
# include <netinet/in.h>
# include <poll.h>
# include <string>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>
# include <vector>

class	Channel;

class Server
{
  private:
	char *_port;
	std::string _password;
	int _serverfd;
	struct addrinfo _specs;
	struct addrinfo *_servinfo;
	socklen_t _adlen;
	std::map<int, Client *> _clients;
	std::map<std::string, Channel *> _channels;
	std::map<std::string, Command *> _commands;

	int setUpSocket(); // private because of encapsulation

  public:
	// CONSTRUCTORS AND DESTRUCTOR

	Server();
	Server(char *port, const std::string &password);
	Server(const Server &other);
	Server &operator=(const Server &other);
	~Server();

	// GETTERS AND SETTERS

	char *getPort() const;
	std::string getPassword() const;
	int getServerfd() const;
	std::map<int, Client *> &getClients();

	Channel *getChannel(const std::string &channel);
	Channel *createChannel(const std::string &channel, Client &creator);

	Client *getUser(const std::string &nick);

	// COMMANDS HANDLERS

	void handleJoin(Client &client, const std::vector<std::string> &args);
	void handlePart(Client &client, const std::vector<std::string> &args);
	void handlePrivMsg(Client &client, const std::vector<std::string> &args);
	void handleKick(Client &client, const std::vector<std::string> &args);
	void handleInvite(Client &client, const std::vector<std::string> &args);
	void handleTopic(Client &client, const std::vector<std::string> &args);
	void handleMode(Client &client, const std::vector<std::string> &args);

	// Client related Commands
	void handlePass(Client &client, const std::vector<std::string> &args);
	void handleNick(Client &client, const std::vector<std::string> &args);
	void handleUser(Client &client, const std::vector<std::string> &args);
	void handleQuit(Client &client, const std::vector<std::string> &args);

	bool isNickTaken(const std::string &nickname) const;
	void registerClient(Client &client);

	// HELPER FUNCTIONS

	const std::vector<Client *> getUserArguments(const std::vector<std::string> &args);
	const std::vector<Channel *> getChanArguments(const std::vector<std::string> &args);

	// MEMBER FUNCTIONS

	void loop(); // main loop
	bool parse(std::string msg, Client *client);
	void broadcastMsg(Client &client, Channel *channel, const std::string &msg);
};

#endif