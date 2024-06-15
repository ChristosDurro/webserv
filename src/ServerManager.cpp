/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 12:46:33 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/15 12:09:47 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerManager.hpp"

ServerManager::ServerManager(Config const &config) : _maxFD(0), _config(config)
{
	// Initialize servers and sockets
	std::vector<ServerConfig> servers = _config.getServers();
	isON = false;
	for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		if (it->getHost().empty())
		{
			return;
		}

		std::set<int> ports = it->getPorts();
		for (std::set<int>::iterator portIT = ports.begin(); portIT != ports.end(); ++portIT)
		{
			ServerSocket *serverSocket = new ServerSocket(static_cast<int>(*portIT), it->getIP(), *it);
			_serverSockets.push_back(serverSocket);
			if (serverSocket->getFD() > _maxFD)
			{
				_maxFD = serverSocket->getFD();
			}
		}
	}
}

ServerManager::~ServerManager()
{
	std::vector<ServerSocket *>::iterator serverIT = _serverSockets.begin();
	for (; serverIT != _serverSockets.end(); serverIT++)
	{
		delete *serverIT;
	}
	_serverSockets.clear();

	std::map<int, ClientSocket *>::iterator clientIT = _clientSockets.begin();
	for (; clientIT != _clientSockets.end(); clientIT++)
	{
		close(clientIT->first);
		delete clientIT->second;
	}
	_clientSockets.clear();
}

void ServerManager::run()
{
	fd_set readFDs, writeFDs;
	struct timeval timeout;

	while (!g_signalPressed)
	{
		timeout.tv_sec = 10; // 10 seconds timeout
		timeout.tv_usec = 0;
		FD_ZERO(&readFDs);
		FD_ZERO(&writeFDs);

		for (std::vector<ServerSocket *>::iterator it = _serverSockets.begin(); it != _serverSockets.end(); ++it)
		{
			FD_SET((*it)->getFD(), &readFDs);
		}

		for (std::map<int, ClientSocket *>::iterator it = _clientSockets.begin(); it != _clientSockets.end(); ++it)
		{
			FD_SET(it->first, &readFDs);
			if (it->second->hasResponseToSend())
			{
				FD_SET(it->first, &writeFDs);
			}
		}

		int numReady = select(_maxFD + 1, &readFDs, &writeFDs, NULL, &timeout);
		if (numReady < 0)
		{
			FD_ZERO(&readFDs);
			FD_ZERO(&writeFDs);
			throw std::runtime_error("select() failed");
		}
		// else if (numReady == 0)
		// {
		// 	std::cout << "Server inactive for timeout period" << std::endl;
		// 	continue;
		// }

		// Check server sockets for new connections
		for (std::vector<ServerSocket *>::iterator it = _serverSockets.begin(); it != _serverSockets.end(); ++it)
		{
			if (FD_ISSET((*it)->getFD(), &readFDs))
			{
				handleNewConnection((*it)->getFD());
			}
		}

		// Check client sockets for requests or responses
		for (std::map<int, ClientSocket *>::iterator it = _clientSockets.begin(); it != _clientSockets.end();)
		{
			int clientFD = it->first;
			ClientSocket *clientSocket = it->second;

			if (FD_ISSET(clientFD, &readFDs))
			{
				handleRequest(clientFD, *clientSocket);
			}

			if (FD_ISSET(clientFD, &writeFDs))
			{
				if (clientSocket->hasResponseToSend())
				{
					clientSocket->sendResponse();
					if (clientSocket->isDone())
					{
						std::map<int, ClientSocket *>::iterator next = it;
						++next;
						close(clientFD);
						_clientSockets.erase(it);
						delete clientSocket;
						it = next;
						continue;
					}
				}
			}


				// std::cout << "i'm going through here" << std::endl;
				if (clientSocket->hasCommunicationTimeout())
				{
					std::cout << RED << "Client " << clientFD << " has timed out" << DEFAULT << std::endl;
					std::map<int, ClientSocket *>::iterator next = it;
					++next;
					close(clientFD);
					_clientSockets.erase(it);
					delete clientSocket;
					it = next;
					continue;
				}
			if (clientSocket->getDisconnection())
			{

				std::cout << RED << "Client " << clientFD << " has disconnected" << DEFAULT << std::endl;
				std::map<int, ClientSocket *>::iterator next = it;
				++next;
				close(clientFD);
				_clientSockets.erase(it);
				delete clientSocket;
				it = next;
				continue;
			}
			++it;
		}
	}
}

void ServerManager::handleNewConnection(int serverFD)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int clientFD = accept(serverFD, (struct sockaddr *)&client_addr, &client_len);
	if (clientFD < 0)
	{
		perror("accept");
		return;
	}
	_clientSockets[clientFD] = new ClientSocket(clientFD);
	if (clientFD > _maxFD)
	{
		_maxFD = clientFD;
	}
}

void ServerManager::handleRequest(int clientFD, ClientSocket &client)
{
	std::cout << std::endl
			  << std::endl
			  << std::endl
			  << "----------- NEW REQUEST -----------" << std::endl;
	static int i = 0;
	std::cout << "i now: " << i++ << std::endl;
	char buffer[4096] = {0};

	int bytesReceived = recv(clientFD, buffer, sizeof(buffer), 0);

	if (bytesReceived <= 0)
	{
		client.setDisconnect(true);
		return;
	}
	std::string requestStr(buffer, bytesReceived);

	std::cout << "########################### REQUEST GOT:" << std::endl;
	std::cout << requestStr << std::endl;
	if (!client.getRequestParsed())
	{
		// std::cout << "got in parse http headers" << std::endl;
		client.parseHttpHeaders(requestStr);
	}

	if (!client.getRequestIsReady())
	{

		// std::cout << "got in parse request body" << std::endl;
		client.parseHttpBody(requestStr);
	}

	// std::cout << "REACHED HERE" << std::endl;
	HttpRequest req = client.getRequest();
	// std::map<std::string, std::string>::iterator it = req.headers.begin();
	// std::cout << "Client Headers: " << std::endl;
	// for (; it != req.headers.end(); it++)
	// {
	// 	std::cout << it->first << ": " << it->second << std::endl;
	// }
	if (client.getRequestIsReady())
	{
		// std::cout << "HTTP Request: Client ID: " << client.getFD() << " Method: " << client.

		HttpResponse res = processReq(client.getRequest(), client);
		// std::cout << "Response:" << std::endl << res.body << std::endl;
		client.setResponse(res);
		client.reset();
	}
}

HttpResponse ServerManager::processReq(HttpRequest req, ClientSocket &client)
{
	ServerConfig server = getCorrectServer(req, client); // gets the correct server for each request

	// std::cout << std::endl << "Server Got: " << std::endl;
	// std::cout << "REQ URI: " << server.getMaxBodySize() << std::endl;
	std::cout << BLUE << "HTTP Request: " << req.method << " " << req.uri << " | From client [" << client.getFD() << "] | To: " << server.getServerName() << DEFAULT << std::endl;

	// if there's cgi to execute, do that
	if (req.uri.find("/script/") != std::string::npos)
	{
		std::cout << "EXECUTING CGI..." << std::endl;
		return processCgiReq(req, client, server);
	}
	return executeReq(server, req);
}
HttpResponse ServerManager::executeReq(ServerConfig server, HttpRequest req)
{
	Location location = getCorrectLocation(server, req.uri);
	std::cout << "location got: " << location << std::endl;
	// std::cout << "redirectopU				RL " << redirectionUrl << server.getRoot() << std::endl;

	if (req.version != "HTTP/1.1")
		return createHttpResponse(505, server.getErrorPage(505));

	std::map<std::string, std::string>::iterator it = req.headers.find("Content-Length");
	if (it != req.headers.end() && !server.bodySizeAllowed(toInt(it->second)))
		return createHttpResponse(413, server.getErrorPage(413));

	if (!server.bodySizeAllowed(req.body.size()))
		return createHttpResponse(413, server.getErrorPage(413));

	std::vector<std::string> methods = location.getMethods();
	if (std::find(methods.begin(), methods.end(), req.method) == methods.end())
		return createHttpResponse(405, server.getErrorPage(405));

	try
	{
		if (req.method == "GET" && (location.getAutoIndex() == true || isON == true))
		{
			isON = true;
			std::cout << "trueeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee autoindex " << location.getAutoIndex()<<  std::endl;	
			return handleGetRequestAuto(server, location, req);
		}
		else if (req.method == "GET")
		{
			std::cout << "falseeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee autoindex " <<location.getAutoIndex() <<  std::endl;	
			return handleGetRequest(server, location, req);
		}
		else if (req.method == "POST")
			return handlePostRequest(server, req, location);
		else if (req.method == "DELETE")
			return handleDeleteRequest(server, req);
	}
	catch (std::exception &e)
	{
		std::cout << RED << "Server request handling exception!" << DEFAULT << std::endl;
	}

	return createHttpResponse(500, server.getErrorPage(500));
}

HttpResponse ServerManager::handleGetRequest(ServerConfig server, Location location, HttpRequest req)
{
    // Construct the full filepath based on server configuration and request
	std::string serverRoot = server.getRoot().substr(0, server.getRoot().length() - 1);
	std::cout << "server root: " << serverRoot << std::endl;
	std::string filepath = fullPath("public/", server.getRoot() + location.getIndexFile());
	std::cout << "filepath in get request: " << filepath << std::endl;

	if (endsWith(req.uri, ".html"))
		filepath = fullPath("public/", serverRoot + req.uri);
	// if (req.uri(".html"))
    // Check for redirection based on location configuration
    if (!location.getRedirection().empty())
        return createHttpResponse(301, location.getRedirection());

    // Extract parent directory path from server root
    // std::string pages = server.getRoot().substr(0, server.getRoot().find_last_of("/"));

    struct stat fileInfo;

    // Check if the file or directory at filepath exists and retrieve its information
    if (stat(filepath.c_str(), &fileInfo) != 0 || filepath.find(req.uri) == std::string::npos)
    {
        // If filepath does not exist, log and return 404 Not Found response
		std::cout << "Filepath does not exist: " << filepath << std::endl;
		return createHttpResponse(404, server.getErrorPage(404));
    }

    // If the filepath exists, check its type using stat
    if (S_ISREG(fileInfo.st_mode))
    {
        // If it's a regular file, log and return 200 OK response
        std::cout << "Requested URI is a regular file: " << req.uri << std::endl;
        return createHttpResponse(200, filepath);
    }

    return createHttpResponse(500, server.getErrorPage(500));
}

HttpResponse ServerManager::handleGetRequestAuto(ServerConfig server, Location location, HttpRequest req)
{
	std::cout << "HEEEEEEEEEEEEEEEEEEEEEEEEEEEREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEe" << std::endl;
	std::string serverRoot = server.getRoot().substr(0, server.getRoot().length() - 1);
	std::cout << "server root: " << serverRoot << std::endl;
    std::string filepath = fullPath("public/", serverRoot + req.uri);
    // std::string filepath = fullPath("public/", server.getRoot() + location.getIndexFile());
    // Check for redirection

    if (!location.getRedirection().empty())
        return createHttpResponse(301, location.getRedirection());

    std::string pages = server.getRoot().substr(0, server.getRoot().find_last_of("/"));

    // Check if the requested URI corresponds to a directory
    if (isDirectory(filepath))
    {
        std::cout << "Requested URI is a directory: " << req.uri << std::endl;
        return createAutoindex(server, req.uri, location.getRoute());
    }

    struct stat fileInfo;
    // Check if the filepath exists and is a regular file
    if (stat(filepath.c_str(), &fileInfo) != 0)
    {
		if (stat(fullPath("public/" + serverRoot, location.getIndexFile()).c_str(), &fileInfo) != 0) {

        	std::cout << "Filepath does not exist: " << filepath << std::endl;
        	return createHttpResponse(404, server.getErrorPage(404));
		}
		return createHttpResponse(200, fullPath("public/" + serverRoot, location.getIndexFile()));
    }

    // If it's a regular file, return 200 OK
    if (S_ISREG(fileInfo.st_mode))
    {
        std::cout << "Requested URI is a regular file: " << req.uri << std::endl;
        return createHttpResponse(200, filepath);
    }

    // Default to 403 Forbidden if neither a directory nor a regular file
    return createHttpResponse(403, server.getErrorPage(403));
}
HttpResponse ServerManager::handlePostRequest(ServerConfig server, HttpRequest req, Location location)
{

	if (req.headers["Content-Type"].find("multipart/form-data") != 0)
	{
		std::cout << "Post content type isn't multipart/form-data" << std::endl;
		return createHttpResponse(204, "");
	}
	std::string root = fullPath(server.getRoot(), location.getRoute());
	std::string boundry = getBoundary(req);

	if (req.body.find(boundry) == std::string::npos || req.body.find("filename=\"") == std::string::npos)
	{
		throw std::runtime_error("couldn't find boundry or filename");
	}

	// Get file name
	std::string filename;
	size_t nameStart = req.body.find("filename=\"") + 10;
	size_t nameEnd = req.body.find("\"", nameStart);

	filename = req.body.substr(nameStart, nameEnd - nameStart);

	// Trim to file content only
	req.body = req.body.substr(req.body.find("\r\n\r\n") + 4);

	if (req.body.find("\r\n" + boundry + "--") != std::string::npos)
		req.body = req.body.substr(0, req.body.find("\r\n" + boundry + "--"));

	std::string path = fullPath("public/uploads/", filename);
	std::ofstream outputFile(path.c_str());

	if (!outputFile.is_open())
		throw std::runtime_error("failed to create file " + path);
	outputFile << req.body;
	outputFile.close();

	return createHttpResponse(204, "");
}

HttpResponse ServerManager::handleDeleteRequest(ServerConfig server, HttpRequest req)
{
	// Get file name
	std::string path = fullPath("public/uploads/", req.body);

	if (::remove(path.c_str()) == 0)
		return createHttpResponse(204, "");

	std::cout << "Delete request: " << path << " not found";
	return createHttpResponse(404, server.getErrorPage(404));
}

Location ServerManager::getCorrectLocation(ServerConfig server, std::string uri)
{
	std::map<std::string, Location> locations = server.getLocations();
	std::map<std::string, Location>::iterator locationIT = locations.begin();


	for (; locationIT != locations.end(); locationIT++)
	{
		std::cout << locationIT->second << std::endl;
		if (locationIT->first == uri)
			return locationIT->second;
	}

	if (uri == "/") {
		throw std::runtime_error("root location '/' not found");
	}

	// Recursively match the less complete uri
	size_t endPos = uri.find_last_of('/');
	uri = endPos == 0 ? "/" : uri.substr(0, uri.find_last_of('/'));
	return getCorrectLocation(server, uri);
}

// Location ServerManager::getCorrectLocation(ServerConfig server, std::string uri)
// {
// 	std::map<std::string, Location> locations = server.getLocations();
// 	std::map<std::string, Location>::iterator locationIT = locations.begin();


// 	for (; locationIT != locations.end(); locationIT++)
// 	{
// 		std::cout << locationIT->second << std::endl;
// 		if (locationIT->first == uri)
// 			return locationIT->second;
// 		std::string redir = locationIT->second.getRedirection();
// 		if (!redir.empty())
// 			if (redir.find_first_of("/") != std::string::npos)
// 				redir = redir.substr(redir.find_first_of("/") + 1);
			
// 		size_t pos = uri.find_first_of("/") + 1;
		
// 		if (pos != std::string::npos && redir.find(uri.substr(uri.find_first_of("/") + 1)) != std::string::npos) {
// 			std::cout << "############### REDIR ##############" << std::endl << "REDIR: " <<  redir << " | URI: " << uri << std::endl << std::endl << std::endl << std::endl;
// 			return locationIT->second;

// 		}
// 	}

// 	Location newLocation;

// 	newLocation.initLocationVals();
// 	newLocation.setMethod("GET");
// 	if (endsWith(uri, ".html") || endsWith(uri, ".cpp") || endsWith(uri, ".png") || endsWith(uri, ".gif"))
// 	{
// 		newLocation.setIndexFile(uri.substr(uri.find_last_of("/")));
// 		std::cout << "new location index file: " << newLocation.getIndexFile() << std::endl;
// 		return newLocation;
// 	}
// 	newLocation.setIndexFile("error_pages/404.html");
// 	return newLocation;
// }

ServerConfig ServerManager::getCorrectServer(HttpRequest req, ClientSocket &client)
{
	(void)client;
	if (req.headers.find("Host") == req.headers.end())
		throw std::runtime_error("No host header");

	std::string host = req.headers.find("Host")->second;
	int port;
	size_t colonPos = host.find(":");

	if (colonPos != std::string::npos)
	{
		port = toInt(host.substr(colonPos + 1));
		host = host.substr(0, colonPos);
	}

	std::vector<ServerConfig> servers = _config.getServers();

	for (size_t i = 0; i < servers.size(); i++)
	{
		// std::cout << "Server host: " << servers[i].getHost() << " Host from req: " << host << std::endl;

		if (servers[i].getServerName() == host)
			return servers[i];

		std::set<int> ports = servers[i].getPorts();
		std::set<int>::iterator portIT = ports.begin();

		for (; portIT != ports.end(); portIT++)
		{
			if (servers[i].getHost() == host && *portIT == port)
				return servers[i];
		}
	}
	throw std::runtime_error("Couldn't find correct server!");
}


bool ServerManager::is_valid(Config *config)
{

	std::vector<ServerConfig> servers = config->getServers();
	std::vector<ServerConfig>::iterator serverIT = servers.begin();
	// teo
	std::set<std::pair<std::string, int> > hostPortPairs;

	for (; serverIT != servers.end(); serverIT++)
	{

		std::set<int> ports = serverIT->getPorts();
		// std::set<int>::iterator portIT = ports.begin();

		std::map<std::string, Location> locations = serverIT->getLocations();
		std::map<std::string, Location>::iterator locationIT = locations.begin();

		std::map<int, std::string> errorPages = serverIT->getErrorPages();
		std::map<int, std::string>::iterator errorPageIT = errorPages.begin();

		for (; errorPageIT != errorPages.end(); errorPageIT++)
		{
			if (errorPageIT->second.empty())
			{
				if (errorPageIT->first == 400)
					errorPages[400] = "error_pages/400.html";
				else if (errorPageIT->first == 403)
					errorPages[403] = "error_pages/403.html";
				else if (errorPageIT->first == 404)
					errorPages[404] = "error_pages/404.html";
				else if (errorPageIT->first == 405)
					errorPages[405] = "error_pages/405.html";
				else if (errorPageIT->first == 408)
					errorPages[408] = "error_pages/408.html";
				else if (errorPageIT->first == 411)
					errorPages[411] = "error_pages/411.html";
				else if (errorPageIT->first == 413)
					errorPages[413] = "error_pages/413.html";
				else if (errorPageIT->first == 500)
					errorPages[500] = "error_pages/500.html";
				else if (errorPageIT->first == 501)
					errorPages[501] = "error_pages/501.html";
				else if (errorPageIT->first == 505)
					errorPages[505] = "error_pages/505.html";
			}
		}

		serverIT->setErrorPages(errorPages);

		for (; locationIT != locations.end(); locationIT++)
		{
			if (locationIT->second.getIndexFile().empty())
				locationIT->second.setIndexFile(serverIT->getIndex());
			// std::cout << "LOCATION IT: " << locationIT->second << std::endl;
		}

		serverIT->setLocations(locations);
		if (serverIT->getHost().empty())
		{
			std::cout << "No host found." << std::endl;
			return 0;
		}
		if (ports.empty())
		{
			std::cout << "Server #" << serverIT->getServerNum() << " - No ports configured for the server. " << std::endl;
			return 0;
		}
		if (serverIT->getRoot() == "")
		{
			std::cout << "Server #" << serverIT->getServerNum() << " - Root not found. " << std::endl;
			return 0;
		}
		if (serverIT->getIndex() == "")
		{
			std::cout << "Server #" << serverIT->getServerNum() << " - Index not found. " << std::endl;
			return 0;
		}
		if (serverIT->getLocations().empty())
		{
			std::cout << "Server #" << serverIT->getServerNum() << " - No locations configured for the server. " << std::endl;
			return 0;
		}
		bool isDuplicate = false;
		std::set<int>::iterator portIT = ports.begin();
		for (; portIT != ports.end(); ++portIT)
		{
			std::pair<std::string, int> hostPortPair = std::make_pair(serverIT->getHost(), *portIT);
			if (hostPortPairs.find(hostPortPair) != hostPortPairs.end())
			{
				isDuplicate = true;
				break;
			}
		}
		if (isDuplicate)
		{
			std::cout << "Skipping server #" << serverIT->getServerNum() << " due to duplicate host and port combination." << std::endl;
			continue;
		}

		for (portIT = ports.begin(); portIT != ports.end(); ++portIT)
		{
			hostPortPairs.insert(std::make_pair(serverIT->getHost(), *portIT));
		}
	}

	config->setServers(servers);
	return 1;
}

void ServerManager::setConfig(Config config)
{
	_config = config;
}


bool ServerManager::isDirectory(const std::string &path)
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) != 0)
	{
		return false;
	}
	return S_ISDIR(statbuf.st_mode);
}
