/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:56:28 by lde-taey          #+#    #+#             */
/*   Updated: 2025/09/22 16:25:13 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

# include "Declarations.hpp"
# include <set>
# include <string>
# include <sys/socket.h>
#include <iostream>

class Client
{
	private:
		int	_fd;
		int	_nb_chan;
		std::string	_ipAddress;
		std::string	_nick;
		std::string	_buffer;
		std::set<Channel *>	_channels;
		std::string _username;
		std::string _realname;
		std::string	_recvBuffer;
		std::string _sendBuffer;
		bool		_isAuthenticated;

	public:
		// Constructors and Destructor
		Client();
		Client(int fd, const std::string& ip);
		// Client(const Client& oth);
		// Client&	operator=(const Client& oth);
		~Client();
		
		//Getters
		int		getFd() const;
		std::string	getNick() const;
		std::string getUserName() const;
		std::string getRealName() const;
		std::string getRecvBuffer() const;
		std::string getSendBuffer() const;
		bool	isAuthenticated() const;

		// Setters
		void 	setFd(int fd);
		void 	setNick(const std::string& nick);
		void 	setUserName(const std::string& username);
		void 	setRealName(const std::string& realname);
		void 	setRecvBuffer(const std::string& recvbuffer);
		void 	setSendBuffer(const std::string& sendbuffer);
		void 	setIsAuthenticated(bool value);

		void	appendToSendBuffer(const std::string& data);
		void	clearSendBUffer();

		// Member functions
		int	getFd() { return _fd; }
		std::set<Channel *>	getUserChannel()	{ return _channels; }
		bool	hasChannel(Channel *channel)	{ return (_channels.find(channel) != _channels.end()); }
		void	addUserChannel(Channel *channel)	
		{
			if (!hasChannel(channel))
				_channels.insert(channel); 
		}
		void	delUserChannel(Channel *channel)	
		{
			if (hasChannel(channel))
				_channels.erase(channel); 
		}
		int	getNbChannel()	{ return _nb_chan; }
		std::string	getName()	{ return _nick; }
		void	addChannel()	{ _nb_chan++; }
		void	sendMsg(Client &client, std::string &msg)	
		{ 
			if (!msg.empty())
				send(client._fd, msg.c_str(), msg.length(), 0);
		}
		
};

const int MAX_CHANNELS = 10;

#endif
