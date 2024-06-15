/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 13:00:23 by cdurro            #+#    #+#             */
/*   Updated: 2024/05/29 14:48:34 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include "webserv.hpp"
#include "ServerConfig.hpp"

class ServerSocket
{
private:
	int _port;
	int _ip;
	int _fd;

	struct sockaddr_in _socketAddress;

	ServerConfig _config;

public:
	ServerSocket(int port, int ip, ServerConfig config);
	~ServerSocket();

	void socketSetup();
	int getPort() const;
	int getIP() const;
	int getFD() const;
	// struct pollfd getPollFD() const;
	struct sockaddr_in getAddress() const;
};

#endif