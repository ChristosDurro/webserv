/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 13:37:44 by cdurro            #+#    #+#             */
/*   Updated: 2024/05/29 14:48:10 by cdurro           ###   ########.fr       */
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
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	int reuse = 1;

	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reuse, sizeof(reuse)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "Error accepting connection!" << std::endl;
		exit(EXIT_FAILURE);
	}

	memset(&_socketAddress, 0, sizeof(_socketAddress));

	_socketAddress.sin_family = AF_INET;
	_socketAddress.sin_port = htons(_port);
	_socketAddress.sin_addr.s_addr = htonl(_ip);

	if (bind(_fd, (const sockaddr *)&_socketAddress, sizeof(_socketAddress)) < 0)
	{
		perror("bind");
		close(_fd);
		exit(EXIT_FAILURE);
	}

	if (listen(_fd, SOMAXCONN) < 0)
	{
		perror("listen");
		close(_fd);
		exit(EXIT_FAILURE);
	}

	std::cout << "Server FD: " << _fd << " | Server listening on port " << _port << std::endl;
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

// struct pollfd ServerSocket::getPollFD() const {
// 	return _pollFD;
// }

struct sockaddr_in ServerSocket::getAddress() const
{
	return _socketAddress;
}