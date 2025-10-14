/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shkaruna <shkaruna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/09 21:01:19 by spitul            #+#    #+#             */
/*   Updated: 2025/10/14 16:07:21 by shkaruna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

Channel::Channel(const std::string &name, Client *creator)	
	: _name(name), _topic(), _members(), _operators(), _key(), _topic_restrict(false)
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
	if (_limit.active && static_cast<size_t>(_limit.value) >= _members.size())
	{
		const std::string	msg = "Channel limit reached"; //check IRC docs
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
	user.delUserChannel(this);
	if (isOperator(user))
		this->delOperator(user);
	if (isInviteOnly() && isInvited(user))
		this->delInvitation(user);
}
		
void	Channel::delOperator(Client &user)
{
	_operators.erase(&user);
}

bool		Channel::isOperator(Client &user)
{
	if (_operators.find(&user) != _operators.end())
		return true;
	return false;
}

bool		Channel::isMember(Client &user)
{
	if (_members.find(&user) != _members.end())
		return true;
	return false;
}

std::set<Client*>& Channel::getMembers() 
{ 
	return _members; 
}

std::string	Channel::getTopic()
{
	return _topic;
}

void		Channel::setTopic(const std::string &newTopic)
{
	_topic = newTopic;
}

void	Channel::restrictTopic(bool restrict)	
{
	if (restrict)
		_topic_restrict = true;
	else
		_topic_restrict = false;
}

// void	Channel::unrestrictTopic()
// {
// 	_topic_restrict = false;
// }

bool	Channel::hasRestrictedTopic()
{
	return _topic_restrict;
}

void	Channel::setKey(const std::string	&pw)
{
	_key = pw;
}

bool	Channel::hasKey()
{
	if (_key.empty())
		return false;
	return true;
}
bool	Channel::checkKey(const std::string &pw)
{
	if (_key == pw)
		return true;
	return false;
}

bool	Channel::isInviteOnly()
{
	return _invite.active;
}

void	Channel::setInviteOnly(bool enableInvite)
{
	if (enableInvite)
		_invite.active = true;
	else
		_invite.active = false;
}

// void	Channel::unsetInviteOnly()
// {
// 	_invite.active = false;
// }

bool		Channel::isInvited(Client &client)
{
	if (_invite.active)
	{
		if (_invite.clients.find(&client) != _invite.clients.end())
			return true;	
		return false;
	}
	return true;		
}

void	Channel::delInvitation(Client &client)
{
	_invite.clients.erase(&client);
}

void	Channel::addInvitedMember(Client &client)
{
	if (!isInvited(client))
		_invite.clients.insert(&client);
}

Channel::LimitMode	Channel::hasLimit()
{
	return _limit;
}

void	Channel::setLimit(int &newLimit)
{
	_limit.active = true;
	_limit.value = newLimit;	
}

int	Channel::getLimit()
{
	return _limit.value;
}
