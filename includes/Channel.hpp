/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 20:13:00 by spitul            #+#    #+#             */
/*   Updated: 2025/09/08 08:37:23 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <set>
#include "Server.hpp"

class	Channel
{
	private:
		std::string	_name; /*Apart from the the requirement that the
   first character being either '&' or '#'; the only restriction on a
   channel name is that it may not contain any spaces (' '), a control G
   (^G or ASCII 7), or a comma (',' which is used as a list item
   separator by the protocol).*/
		std::vector<std::string>	*_mode;
		std::string	_topic;
		std::set<Client>	_members;
		std::set<Client>	_operators;
	
	public:
		Channel();
		Channel(const Channel &src);
		Channel	&operator=(const Channel &src);
		~Channel();
		
};

#endif
