/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-taey <lde-taey@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 15:30:19 by lde-taey          #+#    #+#             */
/*   Updated: 2025/09/22 15:17:33 by lde-taey         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include "../includes/Declarations.hpp"
#include "../includes/Server.hpp"

int main (int argc, char** argv) 
{
    if (argc != 3) 
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return (1);
    }
    Server server(argv[1], argv[2]);
    server.loop();
    return (0);
}
