/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:25:25 by lde-taey          #+#    #+#             */
/*   Updated: 2025/09/24 15:46:04 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <algorithm>
# include <poll.h>
# include <netinet/in.h>
# include <netdb.h>
# include <cstring>
# include <unistd.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <errno.h>
# include "Declarations.hpp"
# include "Channel.hpp"
# include "Client.hpp"
# include "Commands.hpp"

class Channel;

class Server
{
	private:
		char*					_port;
		std::string				_password;
		int						_serverfd;
		struct addrinfo			_specs;
		struct addrinfo			*_servinfo;
		socklen_t				_adlen;
		std::map<int, Client*>	_clients;
		std::map<std::string, Channel*>	_channels;
		std::map<std::string, Command*> _commands;

		int setUpSocket(); // private because of encapsulation
	
	public:
		// CONSTRUCTORS AND DESTRUCTOR
		
		Server();
		Server(char *port, const std::string& password);
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();

		// GETTERS AND SETTERS

		char*					getPort() const;
		std::string				getPassword() const;
		int						getServerfd() const;
		std::map<int, Client*>	&getClients();

		Channel	*getChannel(const std::string &channel);
		Channel	*createChannel(const std::string &channel, Client &creator);

		// COMMANDS HANDLERS
		
		void	handleJoin(Client &client, const std::vector<std::string> &args);
		void	handlePart(Client &client, const std::vector<std::string> &args);
		void	handlePrivMsg(Client &client, const std::vector<std::string> &args);
		void	handleKick(Client &client, const std::vector<std::string> &args);
		void	handleInvite(Client &client, const std::vector<std::string> &args);
		void	handleTopic(Client &client, const std::vector<std::string> &args);
		void	handleMode(Client &client, const std::vector<std::string> &args);
		
		// MEMBER FUNCTIONS
		
		void loop(); // main loop
		bool parse(std::string msg, Client *client);
};

#endif