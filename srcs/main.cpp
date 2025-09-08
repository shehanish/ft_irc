/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shkaruna <shkaruna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 15:30:19 by lde-taey          #+#    #+#             */
/*   Updated: 2025/08/13 10:28:32 by shkaruna         ###   ########.fr       */
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
    

    Client c;
    
    // if (argc != 3) 
    // {
    //     std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
    //     return (1);
    // }
    std::cout << "Client created!" << std::endl;
    // Server server(argv[1], argv[2]);
    // server.loop();
    return (0);
}
