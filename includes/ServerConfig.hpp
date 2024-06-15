/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttaneski <ttaneski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 13:32:19 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/13 16:24:37 by ttaneski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include "webserv.hpp"
#include "Location.hpp"

class Location;

class ServerConfig {
	private:
		int	_port;
		int	_serverNum;
		int _max_body_size;
		unsigned int _ip;
		
		std::set<int> _ports;
		std::string _serverName;
		std::string _host;
		std::string _root;
		std::string _index;
		std::map<std::string, Location> _locations;
		std::map<int, std::string> _errorPages;

	public:
		ServerConfig();
		ServerConfig(std::stringstream &serverStream, int index, std::map<std::string, Location> locations);
		~ServerConfig();

		bool bodySizeAllowed(int bytes) const;


		void setRoot(const std::string& root);
		void	parsePorts(std::stringstream &ss);
		void	parseErrorPage(std::string str);
		void	parseServerConfigData(std::stringstream &ss);

		void	setErrorPages(std::map<int, std::string> errorPages);
		void	setLocations(std::map<std::string, Location> locations);


		unsigned int convertIP(std::string host);

		int		getPort(void) const;
		int		getServerNum(void) const;
		int		getMaxBodySize() const;
		unsigned int getIP(void) const;
		std::string getServerName(void) const;
		std::string getHost(void) const;
		std::string getRoot(void) const;
		std::string getIndex(void) const;
		std::set<int> getPorts(void) const;
		std::string getErrorPage(int code) const;
		std::map<int, std::string> getErrorPages(void) const;
		std::map<std::string, Location> getLocations(void) const;
};

std::ostream &operator<<(std::ostream &os, ServerConfig &server);


#endif