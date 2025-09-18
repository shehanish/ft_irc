/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spitul <spitul@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 07:38:28 by spitul            #+#    #+#             */
/*   Updated: 2025/09/18 07:43:27 by spitul           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vector>
#include <string>

class Server;
class Client;

class Command
{
	public:
		virtual void execute(Server &server, Client &client, const std::vector<std::string> &args);
		virtual ~Command() {}
};

#endif
