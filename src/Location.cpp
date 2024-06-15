/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 13:51:50 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/11 13:46:44 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Location.hpp"

Location::Location()
{
	// std::cout << "Default location constructor" << std::endl;
}

Location::Location(std::stringstream &ss, std::string path)
{
	// in here i just read the content from the location block, for example it can be something like this from our config file:

	// methods  GET POST DELETE;
	// index index.html;
	//  }
	this->initLocationVals();
	std::string line;

	_route = path;

	// in here i start reading every line from the stream i got, i trim the spaces and extract the information of each line in the location block
	while (std::getline(ss, line))
	{
		trimSpaces(line);
		if (line.empty() || line.find("#") != std::string::npos)
			continue;
		// methods line handler
		if (line.find("methods") != std::string::npos)
		{

			if (line.find("GET") != std::string::npos || line.find("POST") != std::string::npos || line.find("DELETE") != std::string::npos)
			{

				if (line.find("GET") != std::string::npos)
					_methods.push_back("GET");

				if (line.find("POST") != std::string::npos)
					_methods.push_back("POST");

				if (line.find("DELETE") != std::string::npos)
					_methods.push_back("DELETE");
			}
			else
				throw std::runtime_error("Uknown method keyword!");
		}
		// index line handler
		else if (line.find("index") != std::string::npos && (line[5] == ' ' || line[5] == '\t'))
			_indexFile = extractString(line, firstWordLen(line));
		// autoindex line handler
		else if (line.find("autoindex") != std::string::npos && (line[9] == ' ' || line[9] == '\t'))
		{
			std::string value = extractString(line, firstWordLen(line));
			trimSpaces(value);
			if (value == "on")
				_autoindex = true;
			else if (value == "off")
				_autoindex = false;
			else
				throw std::runtime_error("Invalid value for autoindex! Value: " + value);
		}
		// redirection line handler
		else if (line.find("redirection") != std::string::npos)
			_redirection = extractString(line, firstWordLen(line));
		// uploads line handler
		else if (line.find("uploads") != std::string::npos)
			_uploads = extractString(line, firstWordLen(line));
		else
			throw std::runtime_error("Unknow keyword for location!");
	}
}

Location::~Location()
{
}

void Location::initLocationVals()
{
	_route = "";
	_index = "";
	_uploads = "";
	_indexFile = "";
	_redirection = "";
	_autoindex = false;
}

std::string Location::getRoute() const
{
	return _route;
}

std::string Location::getRedirection() const
{
	return _redirection;
}

std::string Location::getIndex() const
{
	return _index;
}

std::string Location::getIndexFile() const
{
	return _indexFile;
}

std::string Location::getUploads() const
{
	return _uploads;
}

bool Location::getAutoIndex() const
{
	return _autoindex;
}

std::vector<std::string> Location::getMethods() const
{
	return _methods;
}

void Location::setMethod(std::string method)
{
	_methods.insert(_methods.end(), method);
}

void Location::setIndexFile(std::string rootIndex)
{
	_indexFile = rootIndex;
}

std::ostream &operator<<(std::ostream &os, Location &location)
{
	os << "Route: " << location.getRoute() << std::endl
	   << "Index: " << location.getIndex() << std::endl
	   << "Uploads: " << location.getUploads() << std::endl
	   << "Index File: " << location.getIndexFile() << std::endl
	   << "Redirection: " << location.getRedirection() << std::endl
	   << "Auto Index: " << location.getAutoIndex() << std::endl;
	if (location.getMethods().size() > 1)
	{
		for (size_t i = 0; i < location.getMethods().size(); i++)
		{
			os << "Method[" << i << "]: " << location.getMethods()[i] << std::endl;
		}
	}
	return os;
}
