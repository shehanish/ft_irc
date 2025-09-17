/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 20:13:00 by spitul            #+#    #+#             */
/*   Updated: 2025/09/16 06:57:29 by spitul           ###   ########.fr       */
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
		
		Channel(const std::string &name, Client *creator);
		Channel(const Channel &src);
		Channel	&operator=(const Channel &src);
		~Channel();

		void		addUser(Client &user);
		void		addOperator(Client &user);
		void		delUser(Client &user);
		void		delOperator(Client &user);
		bool		isOperator(Client &user);
		bool		isMember(Client &user);
		std::string	getTopic();
		void		setTopic(std::string &newTopic);
		void		restrictTopic();
		void		setKey(std::string	&pw);
		bool		hasKey();
		bool		checkKey(std::string &pw);
		bool		isInviteOnly();
		void		setInviteOnly();
		LimitMode	hasLimit();
		void		setLimit(int &limit);
		
	private:
	
		//Client				*_creator;
		std::string			_name;
		std::string			_topic;
		std::set<Client*>	_members;
		std::set<Client*>	_operators;
		
		std::string			_key;
		bool				_invite;
		bool				_topic_restrict;
		LimitMode			_limit;
		
};

#endif
