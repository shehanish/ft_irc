/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shkaruna <shkaruna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:56:28 by lde-taey          #+#    #+#             */
/*   Updated: 2025/09/08 11:02:39 by shkaruna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>

class Client
{
	private:
		int		_fd;
		std::string	_ipAddress;
		std::string	_nick;
		std::string _username;
		std::string _realname;
		std::string	_recvBuffer;
		std::string _sendBuffer;
		bool		_isAuthenticated;
		
	public:
		Client();
		Client(int fd, const std::string& ip);
		Client(const Client& oth);
		Client&	operator=(const Client& oth);
		~Client();

		//Getters
		int		getFd() const;
		std::string	getNick() const;
		std::string getUserName() const;
		std::string getRealName() const;
		std::string getRecvBuffer() const;
		std::string getSendBuffer() const;
		bool	isAuthenticated() const;

		//Setters
		void 	setFd(int fd);
		void 	setNick(const std::string& nick);
		void 	setUserName(const std::string& username);
		void 	setRealName(const std::string& realname);
		void 	setRecvBuffer(const std::string& recvbuffer);
		void 	setSendBuffer(const std::string& sendbuffer);
		void 	setIsAuthenticated(bool value);

		void	appendToSendBuffer(const std::string& data);
		void	clearSendBUffer();
		
};

#endif