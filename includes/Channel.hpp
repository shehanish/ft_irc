/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 20:13:00 by spitul            #+#    #+#             */
/*   Updated: 2025/09/09 21:18:56 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <set>
#include "Server.hpp"

class	Channel
{
	public:

		struct	LimitMode
		{
			bool	active;
			int		value;
			LimitMode()	: active(false), value(0)	{}
		};
		
		Channel(const std::string &name);
		Channel(const Channel &src);
		Channel	&operator=(const Channel &src);
		~Channel();
		
	private:
	
		std::string			_name; /*Apart from the the requirement that the
   first character being either '&' or '#'; the only restriction on a
   channel name is that it may not contain any spaces (' '), a control G
   (^G or ASCII 7), or a comma (',' which is used as a list item
   separator by the protocol).*/
		//std::vector<char>	_modes;
		std::string			_topic;
		std::set<Client*>	_members;
		std::set<Client*>	_operators;
		
		std::string			_key;
		bool				_invite;
		bool				_topic_restrict;
		LimitMode			_limit;
		
};

#endif
