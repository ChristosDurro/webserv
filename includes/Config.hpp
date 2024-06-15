/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:35:05 by cdurro            #+#    #+#             */
/*   Updated: 2024/05/22 14:34:49 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "webserv.hpp"
#include "ServerConfig.hpp"

class ServerConfig;

class Config
{
	private:
		bool _serverOpenFound; // when i find the { in the config file
		std::vector<ServerConfig> _servers;

	public:
		Config();
		Config(std::string config_file);
		~Config();

		void setServers(std::vector<ServerConfig> servers);

		std::vector<ServerConfig> getServers() const;
};

#endif