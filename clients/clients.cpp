/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clients.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shehanihansika <shehanihansika@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 14:29:32 by shkaruna          #+#    #+#             */
/*   Updated: 2025/08/04 16:35:32 by shehanihans      ###   ########.fr       */
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
	_serverAddr.sin_port = htons(port); //Host TO Network Short

	int result = inet_pton(AF_INET, ip.c_str(), &_serverAddr.sin_addr);
	if(result <= 0)
	{
		std::cerr << "Error: Invalid IP address format." << std::endl;
		close(_sock); //cleanup
		_sock = -1;
		return;
	}
	if (connect(_sock, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr)) < 0) {
		std::cerr << "Error: Connection to server failed." << std::endl;
		close(_sock);
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

void Client::sendMessage(const std::string& msg)
{
	if (_sock == -1) {
		std::cerr << "Cannot send message: No active connection." << std::endl;
		return;
	}

	ssize_t sent = send(_sock, msg.c_str(), msg.length(), 0);
	if (sent < 0) {
		perror("send");
	} else {
		std::cout << "Sent message: " << msg << std::endl;
	}
}
