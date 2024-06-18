/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 11:08:40 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/18 11:42:27 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"

HttpRequest parseHttpRequest(std::string &content)
{
	HttpRequest req;
	parseRequestPart(" ", req.method, content);
	parseRequestPart(" ", req.uri, content);
	parseRequestPart("\r\n", req.version, content);
	while (content.find("\r\n") != 0)
	{
		parseRequestHeader(req.headers, content);
	}

	// Remove the blank line after the headers
	content.erase(content.begin(), content.begin() + 2);

	return req;
}

void parseRequestPart(const std::string &sep, std::string &field, std::string &content)
{
	size_t pos = content.find(sep);
	if (pos == std::string::npos)
	{
		throw std::runtime_error("couldn't find '" + sep + "'");
	}

	field = content.substr(0, pos);
	content.erase(content.begin(), content.begin() + pos + sep.length());

	// Check if there's more than one separator (invalid request)
	if (content.find(sep) == 0)
	{
		throw std::runtime_error("extra '" + sep + "'");
	}
}

void parseRequestHeader(std::map<std::string, std::string> &header, std::string &content)
{
	size_t colonPos = content.find(":");
	if (colonPos == std::string::npos)
	{
		throw std::runtime_error("couldn't find ':'");
	}
	std::string key = content.substr(0, colonPos);
	content.erase(content.begin(), content.begin() + colonPos + 1);

	// Handle leading white spaces before value
	size_t valuePos = content.find_first_not_of(" \t");
	if (valuePos == std::string::npos)
	{
		throw std::runtime_error("couldn't find ' '");
	}
	content.erase(content.begin(), content.begin() + valuePos);

	size_t crlfPos = content.find("\r\n");
	if (crlfPos == std::string::npos)
	{
		throw std::runtime_error("couldn't find '\r\n'");
	}
	std::string value = content.substr(0, crlfPos);
	content.erase(content.begin(), content.begin() + crlfPos + 2);

	header[key] = value;
}

HttpResponse createHttpResponse(int code, const std::string &path)
{
	HttpResponse res;

	std::cout << "########## CODE: " << code << " | PATH: " << path << std::endl;

	res.statusCode = code;

	// Handle POST response
	if (code == 204)
		return res;

	// Handle redirect response
	if (code == 301)
	{
		std::cout << "path: " << path << std::endl;
		res.headers["Location"] = path;
		return res;
	}

	try
	{
		res.body = getFileContent(path);
		res.headers["Content-Length"] = toString(res.body.size());
		res.headers["Content-Type"] = getMimeType(getFileExtension(path));
	}
	catch (const std::exception &e)
	{
		std::cout << RED << "createHttpResponse exception: " << DEFAULT << e.what();

		std::ifstream file("public/error_pages/404.html");
		res.body = file.is_open() ? getFileContent("public/error_pages/404.html") : "404 Not found";
		res.headers["Content-Type"] = file.is_open() ? "text/html" : "text/plain";
		res.headers["Content-Length"] = toString(res.body.length());
		file.close();
	}
	return res;
}

HttpResponse createHttpResponse(const std::string &str)
{
	HttpResponse res;

	res.statusCode = 200;
	res.body = str;
	res.headers["Content-Length"] = toString(res.body.size());
	res.headers["Content-Type"] = "text/html";

	return res;
}


HttpResponse createAutoindex(ServerConfig server, const std::string &path, const std::string &root) {
    (void)root;
    std::string response;
    std::string pages = server.getRoot().substr(0, server.getRoot().find_last_of("/"));

    std::cout << "Autoindex is enabled. Generating directory listing..." << std::endl;
    std::string directoryListing = generateDirectoryListing("public/" + server.getRoot() + path, path);
	if (directoryListing.empty())
		return createHttpResponse(403, server.getErrorPage(403));
    std::stringstream response_stream;
    response_stream << "HTTP/1.1 200 OK\r\n"
                    << "Content-Type: text/html\r\n"
                    << "Content-Length: " << directoryListing.length() << "\r\n"
                    << "\r\n"
                    << directoryListing;
    response = response_stream.str();
    std::cout << "path in autoindex: " << "public/" + pages + path << std::endl;

    return createHttpResponse(response);
}


std::string toString(HttpResponse res)
{
	std::string str;
	str.append(std::string("HTTP/1.1") + " ");
	str.append(toString(res.statusCode) + " ");
	str.append(getStatusText(res.statusCode) + "\r\n");

	std::map<std::string, std::string>::iterator it;
	for (it = res.headers.begin(); it != res.headers.end(); it++)
	{
		str.append(it->first + ": ").append(it->second + "\r\n");
	}
	str.append("\r\n").append(res.body);
	return str;
}

std::string getStatusText(int code)
{
	switch (code)
	{
	case 200:
		return "OK";
	case 201:
		return "Created";
	case 204:
		return "No Content";
	default:
		return "Not Found";
	}
}
