/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 13:45:09 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/11 11:06:22 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "webserv.hpp"

class Location
{
private:
	std::string _route;
	std::string _index;
	std::string _uploads;
	std::string _indexFile;
	std::string _redirection;
	std::vector<std::string> _methods;
	bool _autoindex;

public:
	Location();
	Location(std::stringstream &ss, std::string path);
	~Location();

	// std::string &operator[](int index);

	void initLocationVals();

	void setMethod(std::string);
	void setIndexFile(std::string rootIndex);

	std::string getRoute() const;
	std::string getIndex() const;
	std::vector<std::string> getMethods() const;
	std::string getUploads() const;
	std::string getIndexFile() const;
	std::string getRedirection() const;
	// void getMethods() const;
	bool getAutoIndex() const;
};

std::ostream &operator<<(std::ostream &os, Location &location);

#endif