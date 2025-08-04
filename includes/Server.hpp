/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:25:25 by lde-taey          #+#    #+#             */
/*   Updated: 2025/08/01 17:35:12 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>

class Server
{
	private:
		int			_port;
		std::string	_password;
		int			_serversocket;
	
	public:
		Server();
		Server(int port, const std::string& password);
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();

		void run(); // main loop
};

#endif