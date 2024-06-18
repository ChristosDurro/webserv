/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:44:26 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/18 11:42:05 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Config.hpp"

Config::Config()
{
	_serverOpenFound = false;
}

Config::Config(std::string config_file)
{
	std::ifstream inputFile;
	std::stringstream serverStream;
	std::map<std::string, Location> locations;
	_serverOpenFound = false;

	inputFile.open(config_file.c_str());

	if (inputFile.is_open())
	{

		std::string line;

		while (std::getline(inputFile, line))
		{
			// i read each line from config file and trim it's spaces before and after
			trimSpaces(line);
			if (line.empty() || line.find("#") != std::string::npos)
				continue;
			// if i found the opening of a server block, i update the serverFound variable and clear any other previous details i might have from other server blocks before it (for example if i had 2 server blocks i'd already have some data in the stream)
			if (line == "server {")
			{
				_serverOpenFound = true;
				serverStream.str("");
				serverStream.clear();
			}
			// this is basically going in every time after i find the server block opening
			else if (_serverOpenFound)
			{
				// in here i just go to each if block if the line matches the keyword
				if (line.find("location") != std::string::npos)
				{
					std::string locationPath = extractPath(line, line.find("location"), firstWordLen(line));

					if (locationPath.empty())
						throw std::runtime_error("Location path is empty!");

					// in here i extract all the content of a location block
					std::stringstream locationContents(extractLocationConent(line, inputFile));

					// and here i insert it into the locations map which is a <location string, location class(data saved about location)>
					locations.insert(std::make_pair(locationPath, Location(locationContents, locationPath)));
				}
				// this is for when i find the closing block of the server and add all the data i have added into the serverStream to create a serverConfiguration class(configuration for every server block)
				else if (line.find("}") != std::string::npos)
				{
					ServerConfig server(serverStream, _servers.size(), locations);

					_servers.push_back(server);
					locations.clear();
				}
				// here i basically add each line of the server block except for location
				else if (_serverOpenFound)
					serverStream << line << std::endl;
				else
					throw std::runtime_error("Wrong keyword in config file!");
			}
		}
		if (!_serverOpenFound)
			throw std::runtime_error("No server block found!");
	}
	else
	{
		std::cout << RED << "Failed to open file!" << DEFAULT << std::endl;
		throw std::runtime_error("Exiting server...");
	}
}

Config::~Config()
{
}

std::vector<ServerConfig> Config::getServers() const {
	return _servers;
}

void Config::setServers(std::vector<ServerConfig> servers) {
	_servers = servers;
}
