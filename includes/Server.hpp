/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:25:25 by lde-taey          #+#    #+#             */
/*   Updated: 2025/08/06 13:00:34 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <netinet/in.h>
# include <cstring>
# include <unistd.h>
# include <sys/socket.h>
# include <errno.h>

class Server
{
	private:
		int					_port;
		int					_serverfd;
		std::string			_password;
		struct sockaddr_in	_address;
		socklen_t			_adlen;

		int setUpSocket(); // private because of encapsulation
	
	public:
		Server();
		Server(int port, const std::string& password);
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();

		void run(); // main loop
		// int	acceptClient() const;
};

#endif