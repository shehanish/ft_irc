/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 15:30:19 by lde-taey          #+#    #+#             */
/*   Updated: 2025/10/06 18:20:23 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <csignal>
#include <cstdlib>
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Commands.hpp"

volatile sig_atomic_t signalreceived = 0;

void signalhandler(int Sig)
{
    (void)Sig;
    signalreceived = true;
}

int main (int argc, char** argv) 
{
    if (argc != 3) 
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return (1);
    }
    Server server(argv[1], argv[2]);
    signal(SIGINT, signalhandler);
    signal(SIGTERM, signalhandler);
    server.loop();
    return (0);
    
    // Client client(42, "127.0.0.1");
    
    // client.setNick("Shehani");
    // client.setUserName("Shehani123");
    // client.setRealName("Shehai Karunathilake");
    // client.setRecvBuffer("Incomin data..");
    // client.setSendBuffer("Outgoing data..");

    // std::cout << "fd " << client.getFd() <<std::endl;
    // std::cout << "Nick " << client.getNick() << std::endl;
    // std::cout << "User Name " << client.getUserName() << std::endl;
    // std::cout << "Real Name " << client.getRealName() << std::endl;
    // std::cout << "RecvBuffer " << client.getRecvBuffer() << std::endl;
    // std::cout << "SendBuffer " << client.getSendBuffer() << std::endl;
    
    
    // std::cout << "Client created!" << std::endl;
    // client.setNick("NewNick");
    // std::cout << "Updated Nick: " << client.getNick() << std::endl;

    // return (0);
}
