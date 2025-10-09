/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 20:13:00 by spitul            #+#    #+#             */
/*   Updated: 2025/10/09 07:44:57 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

# include <set>
# include "../includes/Client.hpp"
# include <string>
# include <sys/socket.h>

class	Channel
{
	public:

		struct	LimitMode
		{
			bool	active;
			int		value;
			LimitMode()	: active(false), value(0)	{}
		};
		
		struct	InviteMode
		{
			bool	active;
			std::set<Client*>	clients;
			InviteMode()	: active(false), clients()	{}	
		};
		
		Channel(const std::string &name, Client *creator);
		Channel(const Channel &src);
		Channel	&operator=(const Channel &src);
		~Channel();

		void		addUser(Client &user);
		void		addOperator(Client &user, bool setOp);
		void		delUser(Client &user);
		void		delOperator(Client &user);
		bool		isOperator(Client &user);
		bool		isMember(Client &user);
		std::set<Client*>	&getMembers();
		std::string	getTopic();
		void		setTopic(const std::string &newTopic);
		void		restrictTopic(bool restrict);
		//void		unrestrictTopic();
		bool		hasRestrictedTopic();
		void		setKey(const std::string	&pw);
		bool		hasKey();
		bool		checkKey(const std::string &pw);
		bool		isInviteOnly();
		bool		isInvited(Client &client);
		void		setInviteOnly(bool enableInvite);
		//void		unsetInviteOnly();
		void		delInvitation(Client &client);
		void		addInvitedMember(Client &client);
		LimitMode	hasLimit();
		void		setLimit(const int &limit, bool setLimit);
		int			getLimit();
		
	private:
	
		//Client				*_creator;
		std::string			_name;
		std::string			_topic;
		std::set<Client*>	_members;
		std::set<Client*>	_operators;
		
		std::string			_key;
		InviteMode			_invite;
		bool				_topic_restrict;
		LimitMode			_limit;
		
};

#endif
