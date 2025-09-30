/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 15:30:19 by lde-taey          #+#    #+#             */
/*   Updated: 2025/09/30 14:43:52 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <csignal>
#include <cstdlib>
#include "../includes/Server.hpp"

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
    server.loop();
    return (0);
}
