/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shkaruna <shkaruna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 14:21:28 by shkaruna          #+#    #+#             */
/*   Updated: 2025/08/03 16:37:02 by shkaruna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clients.hpp" 

//“I want to talk to a server running on my own computer.”
//On that computer (IP), I want to talk to the IRC server program.

//This creates a client object 
//And then tries to socket 
//prepare the server address
//So far no actual connetion happen unless you also call connect()

int main() {
    Client client("127.0.0.1", 6667); 
    return 0;
}
