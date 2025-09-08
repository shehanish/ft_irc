/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:25:25 by lde-taey          #+#    #+#             */
/*   Updated: 2025/09/08 08:05:00 by spitul           ###   ########.fr       */
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
# include "../includes/Channel.hpp"

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
		std::map<int, Channel>	_channels;

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
		
		// MEMBER FUNCTIONS
		
		void loop(); // main loop
};

#endif