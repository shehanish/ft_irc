/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:25:25 by lde-taey          #+#    #+#             */
/*   Updated: 2025/10/12 13:24:07 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Channel.hpp"
# include "Client.hpp"
# include "Commands.hpp"
# include <algorithm>
# include <errno.h>
# include <iostream>
# include <fcntl.h>
# include <algorithm>
# include <map>
# include <vector>
# include <netdb.h>
# include <netinet/in.h>
# include <netdb.h>
# include <cstring>
# include <csignal>
# include <unistd.h>
# include <poll.h>
# include <string>
# include <sys/socket.h>
# include <sys/types.h>

class	Channel;

struct s_data 
{
	std::string					prefix;
	std::vector<std::string>	args;
};

class Server
{
	private:
		char*							_port;
		std::string						_password;
		int								_serverfd;
		struct addrinfo					_specs;
		struct addrinfo					*_servinfo;
		socklen_t						_adlen;
		std::map<int, Client*>			_clients;
		std::map<std::string, Channel*>	_channels;
		std::map<std::string, Command*> _commands;

		// CONSTRUCTORS
		
		Server(); // private because no server should be constructed without port or password
		Server(const Server& other); // private because copying should not be possible (still OCF)
		Server& operator=(const Server& other); // same ^
		
		int setUpSocket(); // private because of encapsulation
	
	public:
		
		// CONSTRUCTORS AND DESTRUCTOR
		
		Server(char *port, const std::string& password);
		~Server();

	// GETTERS AND SETTERS

		char*						getPort() const;
		std::string					getPassword() const;
		int							getServerfd() const;
		std::map<int, Client*>		&getClients();
		Channel						*getChannel(const std::string &channel);
		Client						*getUser(const std::string &nick);
		const std::vector<Client*>	getUserArguments(const std::vector<std::string> &args);
		const std::vector<Channel*>	getChanArguments(const std::vector<std::string> &args);
		
		// COMMANDS HANDLERS
		
		void	handleJoin(Client &client, const std::vector<std::string> &args);
		void	handlePart(Client &client, const std::vector<std::string> &args);
		void	handlePrivMsg(Client &client, const std::vector<std::string> &args);
		void	handleKick(Client &client, const std::vector<std::string> &args);
		void	handleNick(Client &client, const std::vector<std::string> &args);
		void	handleInvite(Client &client, const std::vector<std::string> &args);
		void	handleTopic(Client &client, const std::vector<std::string> &args);
		void	handleMode(Client &client, const std::vector<std::string> &args);
		void	handlePass(Client &client, const std::vector<std::string> &args);
		void 	handleUser(Client &client, const std::vector<std::string> &args);
		
		bool	isNickTaken(const std::string &nickname) const;
		void	registerClient(Client &client);
		Channel	*createChannel(const std::string &channel, Client &creator);

		
		// MEMBER FUNCTIONS
		
		void serverInit();
		void loop(); // main loop
		bool parse(std::string &msg, Client *client);
		void cleanup();
		void broadcastMsg(Client &client, Channel *channel, const std::string &msg);
};

#endif