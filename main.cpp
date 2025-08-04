/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 15:30:19 by lde-taey          #+#    #+#             */
/*   Updated: 2025/08/04 15:07:24 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "includes/Server.hpp"
#include "clients/clients.hpp"

int main (int argc, char** argv) 
{
    Server server;
    
    if (argc != 3) 
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return (1);
    }
    server = Server(atoi(argv[1]), argv[2]);
    Client client("127.0.0.1", 6667); 
    return (0);
}
