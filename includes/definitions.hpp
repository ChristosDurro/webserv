/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   definitions.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 10:23:53 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/15 11:49:15 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include "webserv.hpp"
#include "ServerConfig.hpp"

#define DEFAULT "\033[37m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define CLIENT_TIMEOUT 5
#define CGI_TIMEOUT 5

class ServerConfig;
struct HttpRequest
{
	std::string method;
	std::string uri;
	std::string version;
	std::string body;
	std::map<std::string, std::string> headers;
};

struct HttpResponse
{
	int statusCode;
	std::string body;
	std::map<std::string, std::string> headers;
};

// ########## HTTP FUNCTIONS #############
void parseRequest(std::string &content);
void parseRequestPart(const std::string &sep, std::string &field, std::string &content);
void parseRequestHeader(std::map<std::string, std::string> &header, std::string &content);

std::string toString(HttpResponse res);
std::string getStatusText(int code);

HttpRequest parseHttpRequest(std::string &content);
HttpResponse createHttpResponse(int code, const std::string &path);
HttpResponse createHttpResponse(const std::string &str);
HttpResponse createAutoindex(ServerConfig server, const std::string &path, const std::string &root);


// #######################################

#endif