/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:57:14 by lde-taey          #+#    #+#             */
/*   Updated: 2025/10/27 14:12:03 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client()
	: _fd(-1), _nick(), _username(), _realname(), _recvBuffer(), 
				_sendBuffer(), _isAuthenticated(false), _isRegistered(false),
				_nb_chan(0), _channels() {};

Client::~Client()
	{
		// std::cout << "Client has destroyed" << std::endl;
	}

Client::Client(int fd, const std::string& ip) 
		: _fd(fd), 
		_ipAddress(ip),
		_nick(),
		_username(),
		_realname(),
		_recvBuffer(),
		_sendBuffer(),
		_isAuthenticated(false),
		_isRegistered(false),
		_nb_chan(0),
		_channels()	{};

Client::Client(const Client& oth) 
		:_fd(oth._fd),
		_ipAddress(oth._ipAddress),
		_nick(oth._nick),
		_username(oth._username),
		_realname(oth._realname),
		_recvBuffer(oth._recvBuffer),
		_sendBuffer(oth._sendBuffer),
		_isAuthenticated(oth._isAuthenticated) {};

Client&	Client::operator=(const Client& oth) 
{
	if(this != &oth)
	{
		_fd = oth._fd;
		_ipAddress = oth._ipAddress;
		_nick = oth._nick;
		_username = oth._username;
		_realname = oth._realname;
		_recvBuffer = oth._recvBuffer;
		_sendBuffer = oth._sendBuffer;
		_isAuthenticated = oth._isAuthenticated;
	}
	return *this;
}

// Getters 

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

const std::string	*Client::getMsg(const std::vector<std::string> &args)
{
	if (!args.empty() && args.back()[0] == ':')
		return	&(args.back());
	return NULL;
}
		

bool	Client::isAuthenticated() const
{
	return _isAuthenticated;
}

// Setters

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
	// std::cout << "Client is Authenticated" << std::endl;
    _isAuthenticated = value;
}

 // Message handling

std::vector<std::string> Client::receiveData(const char *data, size_t len)
{
	_recvBuffer.append(data, len);
	std::vector<std::string> messages;
	
	size_t pos = _recvBuffer.find("\r\n");
	while (pos != std::string::npos)
	{
		std::string msg = _recvBuffer.substr(0, pos);
		messages.push_back(msg);
		_recvBuffer.erase(0, pos + 2);
		pos = _recvBuffer.find("\r\n");
	}
	return (messages);
}

bool Client::flush()
{
	if (_sendBuffer.empty())
		return true;
	
	int sent = send(_fd, _sendBuffer.c_str(), _sendBuffer.size(), 0);
	if (sent < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return false; // try again later
		throw std::runtime_error("Send failed: " + std::string(std::strerror(errno)));
		return false; // actual problem
	}
	_sendBuffer.erase(0, sent);
	return _sendBuffer.empty();
}
bool    Client::setRegistered(bool value)
{
	// std::cout << "Client is Registered!" << std::endl;
	_isRegistered= value; 
	return _isRegistered;
}
// replaced with receiveData() function
void Client::appendToSendBuffer(const std::string& data) 
{
    _sendBuffer += data;
}

void Client::addUserChannel(Channel* channel) {
    if (!channel) return;
    _channels.insert(channel);  // std::set ignores duplicates automatically
}

// Remove a channel from the client's set
void Client::delUserChannel(Channel* channel) {
    if (!channel) return;
    _channels.erase(channel);  // save even if channel is not in the set
}

// Return the number of channels this client is in
int Client::getNbChannel() const {
    return static_cast<int>(_channels.size());
}

void Client::addChannel()
{
	_nb_chan++;
}