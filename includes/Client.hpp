/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:56:28 by lde-taey          #+#    #+#             */
/*   Updated: 2025/10/16 15:39:58 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <set>
# include <string>
# include <cstring>
# include <vector>
# include <sys/socket.h>
# include <sys/socket.h>
# include <iostream>
# include <errno.h>
# include <set>
# include <vector>
# include <sys/types.h>
# include <sys/socket.h>

class Channel; // Forward declaration

const int	MAX_CHANNELS = 10;

class Client
{
	private:
		int 				_fd;
		std::string 		_ipAddress;
		std::string 		_nick;
		std::string 		_username;
		std::string 		_realname;
		std::string 		_recvBuffer; // input from recv
		std::string 		_sendBuffer; // output that needs to go to send
		bool        		_isAuthenticated;
		bool        		_isRegistered;
		int 				_nb_chan;
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
		bool   setRegistered(bool value);
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
		const std::string	*getMsg(const std::vector<std::string> &args);

		// Channel management
		void addUserChannel(Channel *channel);
		void delUserChannel(Channel *channel);
		void addChannel(); // Increment _nb_chan

		// Message handling
		std::vector<std::string> receiveData(const char *data, size_t len);
		bool flush();
		void appendToSendBuffer(const std::string& data);

		//Registration
		bool    isRegistered() const { return _isRegistered; };
};

#endif
