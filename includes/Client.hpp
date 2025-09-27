/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:56:28 by lde-taey          #+#    #+#             */
/*   Updated: 2025/09/25 21:29:35 by spitul           ###   ########.fr       */
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
		int	getNbChannel()	{ return _nb_chan; }
		std::string	getName()	{ return _nick; }
		void	addChannel()	{ _nb_chan++; }
		
		std::string	&getMsg(std::vector<std::string> &args)
		{
			return	*(args.end() - 1);
		}
		
		void	sendMsg(Client &client, std::string &msg)	
		{
			if (!msg.empty())
				send(client._fd, msg.c_str(), msg.size(), 0);
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
