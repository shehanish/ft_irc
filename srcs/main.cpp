/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 15:30:19 by lde-taey          #+#    #+#             */
/*   Updated: 2025/10/28 17:43:36 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
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
    // Validate port: must be a whole number between 1 and 65535
    char *endptr = NULL;
    errno = 0;
    long port = strtol(argv[1], &endptr, 10);
    if (errno != 0 || endptr == argv[1] || *endptr != '\0' || port < 1 || port > 65535)
    {
        std::cerr << "Error: invalid port '" << argv[1] << "'. Port must be a number between 1 and 65535." << std::endl;
        return 1;
    }
    try
    {
        Server server(argv[1], argv[2]);
        signal(SIGINT, signalhandler);
        signal(SIGTERM, signalhandler);
        server.loop();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}
