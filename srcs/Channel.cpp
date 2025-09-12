/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/09 21:01:19 by spitul            #+#    #+#             */
/*   Updated: 2025/09/12 21:35:16 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string &name, Client *creator)	
	: _name(name), _topic(), _members(), _operators(), _key(), 
		_invite(false), _topic_restrict(false)
{
	if (creator)
	{
		_members.insert(creator);
		_operators.insert(creator);
	}
}

Channel::Channel(const Channel &src)
{
	if (this != &src)
		*this = src;
}

Channel&	Channel::operator=(const Channel &src)
{
	if (this != &src)
	{
		_name = src._name;
		_topic = src._topic;
		_members = src._members;
		_operators = src._operators;
		_key = src._key;
		_invite = src._invite;
		_topic_restrict = src._topic_restrict;
		_limit.active = src._limit.active;
		_limit.value = src._limit.value;
	}
	return *this;
}

Channel::~Channel()
{}

void	Channel::addUser(Client &user)
{
	if (_limit.active && _limit.value >= _members.size())
	{
		const std::string	msg = "Channel limit reached";
		send(user.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}
	_members.insert(&user);
}

void	Channel::addOperator(Client &user)
{
	_operators.insert(&user);
}

void	Channel::delUser(Client &user)
{
	_members.erase(&user);
}
		
void	Channel::delOperator(Client &user)
{
	_operators.erase(&user);
}
