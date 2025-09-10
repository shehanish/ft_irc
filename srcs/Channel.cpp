/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/09 21:01:19 by spitul            #+#    #+#             */
/*   Updated: 2025/09/10 07:46:12 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string &name)	
	: _name(name), _topic(), _members(), _operators(), _key(), 
		_invite(false), _topic_restrict(false)
{}

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
