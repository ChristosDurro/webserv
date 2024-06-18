/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 13:37:44 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/18 11:01:45 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerSocket.hpp"

ServerSocket::ServerSocket(int port, int ip, ServerConfig config)
{
	_port = port;
	_ip = ip;
	_config = config;
	socketSetup();
}

ServerSocket::~ServerSocket()
{
}

void ServerSocket::socketSetup()
{

	_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (_fd < 0)
		throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));

	int reuse = 1;

	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reuse, sizeof(reuse)) < 0)
		throw std::runtime_error("setsockopt() failed: " + std::string(strerror(errno)));

	if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Error accepting connection: " + std::string(strerror(errno)));

	memset(&_socketAddress, 0, sizeof(_socketAddress));

	_socketAddress.sin_family = AF_INET;
	_socketAddress.sin_port = htons(_port);
	_socketAddress.sin_addr.s_addr = htonl(_ip);

	if (bind(_fd, (const sockaddr *)&_socketAddress, sizeof(_socketAddress)) < 0)
	{
		close(_fd);
		throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));
	}

	if (listen(_fd, SOMAXCONN) < 0)
	{
		close(_fd);
		throw std::runtime_error("listen() failed: " + std::string(strerror(errno)));
	}

	std::cout << "Server FD: " << _fd << " | Server #" << _config.getServerNum() << " listening on host:port " << _config.getHost() << ":" << _port << std::endl;
}

int ServerSocket::getIP() const
{
	return _ip;
}

int ServerSocket::getPort() const
{
	return _port;
}

int ServerSocket::getFD() const {
	return _fd;
}

struct sockaddr_in ServerSocket::getAddress() const
{
	return _socketAddress;
}