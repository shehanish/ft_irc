/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shkaruna <shkaruna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:25:25 by lde-taey          #+#    #+#             */
/*   Updated: 2025/09/22 17:01:16 by shkaruna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <poll.h>
# include <netinet/in.h>
# include <netdb.h>
# include <cstring>
# include <unistd.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <errno.h>
# include "../includes/Client.hpp"
# include "../includes/Command.hpp"

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
		std::map<int, Client>	_clients;
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
		std::map<int, Client>	&getClients();

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
		
		//Client related Commands
		void	handlePass(Client &client, const std::vector<std::string> &args);
		void	handleNick(Client &client, const std::vector<std::string> &args);
		void	handleUser(Client &client, const std::vector<std::string> &args);
		void	handleQuit(Client &client, const std::vector<std::string> &args);
		
		
		// MEMBER FUNCTIONS
		
		void loop(); // main loop
};

#endif