/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shehanihansika <shehanihansika@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 14:21:28 by shkaruna          #+#    #+#             */
/*   Updated: 2025/08/04 16:35:53 by shehanihans      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clients.hpp" 

//“I want to talk to a server running on my own computer.”
//On that computer (IP), I want to talk to the IRC server program.

//This creates a client object 
//And then tries to socket 
//prepare the server address
//So far no actual connetion happen unless you also call connect()

#include "clients.hpp"

int main() 
{
	Client client("127.0.0.1", 6667); // Connect to local server on port 6667
	client.sendMessage("NICK mynickname\r\n"); // IRC-style NICK command
	return 0;
}

