/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:57:14 by lde-taey          #+#    #+#             */
/*   Updated: 2025/10/03 19:36:03 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client()
	: _fd(-1), _nick(), _username(), _realname(), _recvBuffer(), 
				_sendBuffer(), _isAuthenticated(false) {};

Client::~Client()
	{
		std::cout << "Client has destroyed" << std::endl;
	}

Client::Client(int fd, const std::string& ip) 
		: _fd(fd), 
		_ipAddress(ip),
		_nick(),
		_username(),
		_realname(),
		_recvBuffer(),
		_sendBuffer(),
		_isAuthenticated(false)	{};

// maybe it is too tricky to add a copying possibility? two similar fd's might be a problem
// Client::Client(const Client& oth) 
// 		:_fd(oth._fd),
// 		_ipAddress(oth._ipAddress),
// 		_nick(oth._nick),
// 		_username(oth._username),
// 		_realname(oth._realname),
// 		_recvBuffer(oth._recvBuffer),
// 		_sendBuffer(oth._sendBuffer),
// 		_isAuthenticated(oth._isAuthenticated) {};

// Client&	Client::operator=(const Client& oth) 
// {
// 	if(this != &oth)
// 	{
// 		_fd = oth._fd;
// 		_ipAddress = oth._ipAddress;
// 		_nick = oth._nick;
// 		_username = oth._username;
// 		_realname = oth._realname;
// 		_recvBuffer = oth._recvBuffer;
// 		_sendBuffer = oth._sendBuffer;
// 		_isAuthenticated = oth._isAuthenticated;
// 	}
// 	return *this;
// }

//Getters 

int		Client::getFd() const
{
	return _fd;
}

std::string	Client::getNick() const
{
	return _nick;
}

std::string	Client::getUserName() const
{
	return _username;
}

std::string Client::getRealName() const
{
	return _realname;
}

std::string Client::getRecvBuffer() const
{
	return _recvBuffer;
}

std::string	Client::getSendBuffer() const
{
	return _sendBuffer;
}

bool	Client::isAuthenticated() const
{
	return _isAuthenticated;
}

//Setters
void	Client::setFd(int fd)
{
	_fd = fd;
}

void	Client::setNick(const std::string& nick)
{
	_nick = nick;
}

void	Client::setUserName(const std::string& username)
{
	_username = username;
}

void	Client::setRealName(const std::string& realname)
{
	_realname = realname;
}

void	Client::setRecvBuffer(const std::string& recvbuffer)
{
	_recvBuffer = recvbuffer;
}

void	Client::setSendBuffer(const std::string& sendbuffer)
{
	_sendBuffer = sendbuffer;
}

void Client::setIsAuthenticated(bool value)
{
    _isAuthenticated = value;
}

 // Message handling
 
void Client::appendToSendBuffer(const std::string& data) 
{
    _sendBuffer += data;
}

void Client::clearSendBuffer() 
{
    _sendBuffer.clear();
}

void Client::sendMsg(Client &client, const std::string &msg) 
{
    std::string toSend = msg + "\r\n";
    ::send(client.getFd(), toSend.c_str(), toSend.size(), 0);
}

const std::string* Client::getMsg(const std::vector<std::string> &args)
{
	if (!args.empty() && args.back()[0] == ':')
		return	&(args.back());
	return NULL;
}
