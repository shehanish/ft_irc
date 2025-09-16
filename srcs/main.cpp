/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shkaruna <shkaruna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 15:30:19 by lde-taey          #+#    #+#             */
/*   Updated: 2025/09/16 17:35:44 by shkaruna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

int main (int argc, char** argv) 
{
    (void)argc;
    (void)argv;
    
    std::vector<std::string> response;

    response.push_back("Welcome to the server!");
    response.push_back("Your nick name has been set!");
    response.push_back("You joined #genaral");

    for (size_t i = 0; i < response.size(); ++i)
    {
        std::cout << response[i] << std::endl;
    }
    
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
