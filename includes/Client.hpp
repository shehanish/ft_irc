/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:56:28 by lde-taey          #+#    #+#             */
/*   Updated: 2025/09/22 14:55:49 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

# include "Declarations.hpp"
# include <set>
# include <string>
# include <sys/socket.h>

class Client
{
	public:
		Client();
		Client(int fd);
		// Client(Client const &other);
		// Client &operator=(Client const &other);
		~Client();
		
	
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
			
	private:
		int	_fd;
		int	_nb_chan;
		std::string	_nick;
		std::string	_buffer;
		std::set<Channel *>	_channels;
	
};

const int MAX_CHANNELS = 10;

#endif
