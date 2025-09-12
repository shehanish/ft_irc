/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:56:28 by lde-taey          #+#    #+#             */
/*   Updated: 2025/09/12 21:28:03 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client
{
	public:
		int	getFd() { return _fd; }
			
	private:
		int	_fd;
		std::string	_nick;
		std::string	_buffer;
	
};

#endif
