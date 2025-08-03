/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clients.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shkaruna <shkaruna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 14:29:32 by shkaruna          #+#    #+#             */
/*   Updated: 2025/08/03 16:42:47 by shkaruna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clients.hpp"

//Client constructor: “Create a client that wants to connect to IP 127.0.0.1 and port 6667.”

Client::Client(const std::string& ip, int port)
{
	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(_sock < 0)
	{
		std::cerr << "Error: Clould not create socket" <<std::endl;
		return;
	}
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(port);

	int result = inet_pton(AF_INET, ip.c_str(), &_serverAddr.sin_addr);
	if(result <= 0)
	{
		std::cerr << "Error: Invalid IP address format." << std::endl;
		close(_sock); //cleanup
		_sock = -1;
		return;
	}
	std::cout << "Client created successfully" << std::endl;
}

Client::~Client()
{
	if (_sock != -1) {
		close(_sock);
	}
}