/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 12:11:27 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/15 10:09:42 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "webserv.hpp"
#include "Config.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "CGI.hpp"

class ServerManager
{
private:
	int _maxFD;
	Config _config;
	std::map<int, ClientSocket *> _clientSockets;
	std::vector<ServerSocket *> _serverSockets;
	std::vector<struct pollfd> _pollFDs;

public:
	bool isON;
	ServerManager(Config const &config);
	~ServerManager();

	bool is_valid(Config *config);
	bool isDirectory(const std::string &path);

	void setConfig(Config config);
	void run();
	void handleRequest(int clientFD, ClientSocket &client);
	void handleNewConnection(int serverFD);

	HttpResponse processReq(HttpRequest req, ClientSocket &client);
	HttpResponse executeReq(ServerConfig server, HttpRequest req);
	HttpResponse handleGetRequest(ServerConfig server, Location location, HttpRequest req);
	HttpResponse handleGetRequestAuto(ServerConfig server, Location location, HttpRequest req);
	HttpResponse handlePostRequest(ServerConfig server, HttpRequest req, Location location);
	HttpResponse handleDeleteRequest(ServerConfig server, HttpRequest req);

	Location getCorrectLocation(ServerConfig server, std::string uri);

	ServerConfig getCorrectServer(HttpRequest req, ClientSocket &client);
};

#endif