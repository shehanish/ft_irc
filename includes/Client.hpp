/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:56:28 by lde-taey          #+#    #+#             */
/*   Updated: 2025/09/13 21:54:11 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client
{
	public:
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
			
	private:
		int	_fd;
		std::string	_nick;
		std::string	_buffer;
		std::set<Channel *>	_channels;
	
};

#endif
