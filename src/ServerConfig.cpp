/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttaneski <ttaneski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 14:20:39 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/13 16:23:29 by ttaneski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerConfig.hpp"

ServerConfig::ServerConfig() : _port(0), _serverName(""), _host(""), _root(""), _index("")
{
	// std::cout << "Server Config Constructor called" << std::endl;
}

ServerConfig::ServerConfig(std::stringstream &serverStream, int index, std::map<std::string, Location> locations) : _port(0), _serverNum(index), _max_body_size(-1), _serverName(""), _host(""), _locations(locations)
{
	_errorPages[400] = "";
	_errorPages[403] = "";
	_errorPages[404] = "";
	_errorPages[405] = "";
	_errorPages[408] = "";
	_errorPages[411] = "";
	_errorPages[413] = "";
	_errorPages[500] = "";
	_errorPages[501] = "";
	_errorPages[505] = "";

	std::cout << "------------- SERVER STREAM DATA -------------" << std::endl;
	parseServerConfigData(serverStream);
}

ServerConfig::~ServerConfig()
{
	// std::cout << "Server Config Destructor called" << std::endl;
}

void ServerConfig::setErrorPages(std::map<int, std::string> errorPages)
{
	_errorPages = errorPages;
}


bool ServerConfig::bodySizeAllowed(int bytes) const
{
	// If clientMaxBodySize is not set (-1) or larger
	return getMaxBodySize() == -1 || getMaxBodySize() >= bytes;
}

void ServerConfig::setLocations(std::map<std::string, Location> locations)
{
	_locations = locations;
}

int ServerConfig::getMaxBodySize() const
{
	return _max_body_size;
}

int ServerConfig::getPort(void) const
{
	return _port;
}

int ServerConfig::getServerNum(void) const
{
	return _serverNum;
}

std::string ServerConfig::getServerName(void) const
{
	return _serverName;
}

std::string ServerConfig::getHost(void) const
{
	return _host;
}

std::string ServerConfig::getRoot(void) const
{
	return _root;
}
void ServerConfig::setRoot(const std::string& root) {
        _root = root;
    }
std::string ServerConfig::getIndex(void) const
{
	return _index;
}

std::string ServerConfig::getErrorPage(int code) const
{
	std::map<int, std::string>::const_iterator it = _errorPages.find(code);
	return it == _errorPages.end() ? "" : fullPath("public/", it->second);
}


std::map<int, std::string> ServerConfig::getErrorPages(void) const
{
	return _errorPages;
}

std::map<std::string, Location> ServerConfig::getLocations(void) const
{
	return _locations;
}

std::set<int> ServerConfig::getPorts(void) const
{
	return _ports;
}

void ServerConfig::parseServerConfigData(std::stringstream &ss)
{
	std::cout << ss.str() << std::endl;
	std::string line;
	std::stringstream portStream;

	while (getline(ss, line))
	{
		// this is for all other listen ports (from 2nd and below)
		if (line.find("listen") != std::string::npos && _port != 0)
		{
			portStream << line << std::endl; // i add the whole line (listen 8002; for example) to the portStream
		}
		// this is for the first listen port that we encounter in the config file
		else if (line.find("listen") != std::string::npos)
		{
			std::stringstream splitPort(line);
			std::string listen, portStr, hostStr;

			splitPort >> listen >> portStr;

			size_t colonPos = portStr.find(":");
			if (colonPos != std::string::npos)
			{
				hostStr = portStr.substr(0, colonPos);
				portStr = portStr.substr(colonPos + 1, portStr.length() - 1);
				_port = atoi(portStr.c_str());
				_ports.insert(_port);
				if (_host.empty())
				{
					_host = hostStr;
					_ip = convertIP(_host);
				}
				// std::cout << "HOST STR: " << hostStr << std::endl;
				// std::cout << "PORT STR: " << portStr << std::endl;
			}
			else
			{

				_port = extractInt(line, firstWordLen(line));
				_ports.insert(_port);
			}

			// i get the port number and put it into the ports set
			// if ()
		}
		// extract server_name value from config file
		else if (line.find("server_name") != std::string::npos)
			_serverName = extractString(line, firstWordLen(line));
		// extract host value from config file and save it's IP
		else if (line.find("host") != std::string::npos)
		{
			_host = extractString(line, firstWordLen(line));
			_ip = convertIP(_host);
		}
		// extract root value from config file
		else if (line.find("root") != std::string::npos)
			_root = extractString(line, firstWordLen(line));
		// extract index (basically file to serve as default i think) value from config file
		else if (line.find("index") != std::string::npos && line.find("error_page") == std::string::npos)
			_index = extractString(line, firstWordLen(line));
		// extract error_page (basically default error page file) value from config file
		else if (line.find("error_page") != std::string::npos)
			parseErrorPage(extractString(line, firstWordLen(line)));
		else if (line.find("client_max_body_size") != std::string::npos)
			_max_body_size = extractInt(line, firstWordLen(line));
		else
			throw std::runtime_error("Unknown keyword for server config!");
	}
	// std::cout << "REACHED HERE PL?" << std::endl;
	// basically from before where i added each extra listen port to the stream, i now extract them and put them in the set
	if (!portStream.str().empty())
		parsePorts(portStream);
}

void ServerConfig::parsePorts(std::stringstream &ss)
{
	// std::cout << "------- PORT STREAM -------" << std::endl;
	// std::cout << ss.str() << std::endl;

	std::string line;

	while (getline(ss, line))
		_ports.insert(extractInt(line, firstWordLen(line)));
}

void ServerConfig::parseErrorPage(std::string str)
{
	std::istringstream iss(str);
	std::string path;
	int statusCode;

	iss >> statusCode >> path;
	_errorPages[statusCode] = path;
}

std::ostream &operator<<(std::ostream &os, ServerConfig &server)
{
	os << "Main Port: " << server.getPort() << std::endl
	   << "Server #: " << server.getServerNum() << std::endl
	   << "Server Name: " << server.getServerName() << std::endl
	   << "Host: " << server.getHost() << std::endl
	   << "IP Address: " << server.getIP() << std::endl
	   << "Root: " << server.getRoot() << std::endl
	   << "Index: " << server.getIndex() << std::endl;

	std::set<int> ports = server.getPorts();
	os << std::endl
	   << "All Ports:" << std::endl;
	for (std::set<int>::iterator it = ports.begin(); it != ports.end(); it++)
	{
		os << "Port # " << *it << std::endl;
	}

	std::map<std::string, Location> locations = server.getLocations();
	os << std::endl
	   << "Locations:" << std::endl;
	for (std::map<std::string, Location>::iterator it = locations.begin(); it != locations.end(); it++)
	{
		os << "Location first: " << it->first << std::endl
		   << "Location second: " << it->second << std::endl;
	}

	std::map<int, std::string> errorPages = server.getErrorPages();
	os << std::endl
	   << "All Error Pages:" << std::endl;
	for (std::map<int, std::string>::iterator it = errorPages.begin(); it != errorPages.end(); it++)
	{
		os << "Error Page first: " << it->first << std::endl
		   << "Error Page second: " << it->second << std::endl;
	}
	return os;
}

unsigned int ServerConfig::convertIP(const std::string host)
{
	std::vector<unsigned int> parts;
	std::istringstream ipStream(host);
	std::string part;
	unsigned int ip;
	int count = 0;

	while (std::getline(ipStream, part, '.'))
	{
		count++;
		unsigned int num = static_cast<unsigned int>(atoi(part.c_str()));
		parts.push_back(num);
	}

	if (count != 4)
		throw(std::runtime_error("ServerConfig: Error, IP address has invalid format"));

	ip = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];

	return (ip);
}

unsigned int ServerConfig::getIP(void) const
{
	return _ip;
}