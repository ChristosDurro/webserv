/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttaneski <ttaneski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 12:23:34 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/14 16:15:09 by ttaneski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include "includes/webserv.hpp"
#include "includes/Location.hpp"
#include "includes/ServerConfig.hpp"
#include "includes/Config.hpp"
#include "includes/ServerSocket.hpp"
#include "includes/ServerManager.hpp"

bool g_signalPressed = false;

void signalHandler(int sigNum)
{
	if (sigNum == SIGINT)
	{
		std::cout << "Gracefully exiting server..." << std::endl;
		g_signalPressed = true;
	}
}

int main(int argc, char **argv)
{

	if (argc > 2)
	{
		std::cout << RED << "Wrong number of arguments!" << DEFAULT << std::endl;
		return 1;
	}
	std::string config_file = argv[1] ? argv[1] : "default.conf";
	signal(SIGINT, &signalHandler);

	try
	{

		Config config(config_file);
		ServerManager manager(config);
		// std::string errorMsg;
		// After parsing the config file, i basically start listening and binding ports to each server
		if (!manager.is_valid(&config))
		{
			// Gotta clear everything
			return 1;
		}
		manager.setConfig(config);
		std::cout << "Server Manager start" << std::endl;
		manager.run();
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}