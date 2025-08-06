/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:25:25 by lde-taey          #+#    #+#             */
/*   Updated: 2025/08/06 16:09:27 by lde-taey         ###   ########.fr       */
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
# include "../clients/clients.hpp"

class Server
{
	private:
		char				*_port;
		int					_serverfd;
		std::string			_password;
		struct addrinfo		_specs;
		struct addrinfo		*_servinfo;
		socklen_t			_adlen;

		int setUpSocket(); // private because of encapsulation
	
	public:
		Server();
		Server(char *port, const std::string& password);
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();

		void run(); // main loop
		// int	acceptClient() const;
};

#endif