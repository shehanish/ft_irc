/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clients.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shehanihansika <shehanihansika@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 14:29:29 by shkaruna          #+#    #+#             */
/*   Updated: 2025/08/04 16:35:03 by shehanihans      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <netinet/in.h> // needed for sockaddr_in
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

class Client
{
	private:
		int _sock;
		struct sockaddr_in _serverAddr;
		
	public:
		Client(const std::string& ip, int port);
		~Client();

		void sendMessage(const std::string& msg);

	
};

#endif