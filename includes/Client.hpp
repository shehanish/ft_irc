/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:56:28 by lde-taey          #+#    #+#             */
/*   Updated: 2025/10/02 18:45:16 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

# include <set>
# include <string>
# include <sys/socket.h>
#include <iostream>
#include <set>
#include <sys/types.h>
#include <sys/socket.h>

class Channel; // Forward declaration

const int MAX_CHANNELS = 10;

class Client
{
	private:
		int _fd;
		std::string _ipAddress;
		std::string _nick;
		std::string _username;
		std::string _realname;
		std::string _recvBuffer;
		std::string _sendBuffer;
		bool        _isAuthenticated;
		bool        _isRegistered;
		int _nb_chan;
		std::set<Channel *> _channels;

	public:
		// Constructors & Destructor
		Client();
		Client(int fd, const std::string& ip);
		Client(const Client& oth);
		Client& operator=(const Client& oth);
		~Client();

		// Getters
		int getFd() const;
		std::string getNick() const;
		std::string getUserName() const;
		std::string getRealName() const;
		std::string getRecvBuffer() const;
		std::string getSendBuffer() const;
		bool isAuthenticated() const;
		int getNbChannel() const;
		std::set<Channel *> getUserChannel() const;
		bool hasChannel(Channel *channel) const;

		// Setters
		void setFd(int fd);
		void setNick(const std::string& nick);
		void setUserName(const std::string& username);
		void setRealName(const std::string& realname);
		void setRecvBuffer(const std::string& recvbuffer);
		void setSendBuffer(const std::string& sendbuffer);
		void setIsAuthenticated(bool value);

		// Channel management
		void addUserChannel(Channel *channel);
		void delUserChannel(Channel *channel);
		void addChannel(); // Increment _nb_chan

		// Message handling
		void appendToSendBuffer(const std::string& data);
		void clearSendBuffer();
		void sendMsg(Client &client, const std::string &msg);

		//Registration
		bool    isRegistered() const { return _isRegistered; }
		bool    setRegistered(bool value) {_isRegistered = value; return _isRegistered;}
};

#endif
