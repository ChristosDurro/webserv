/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 10:31:40 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/15 12:07:53 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "ClientSocket.hpp"

HttpResponse processCgiReq(HttpRequest req, ClientSocket &client, ServerConfig &server);
std::map<std::string, std::string> getCgiEnv(HttpRequest req, ClientSocket &client, ServerConfig &server);
std::string getScriptName(const std::string& uri);
std::string getQueryString(std::string &uri);
std::string executeCgi(std::map<std::string, std::string> envs, std::string body, ServerConfig server);
char** getEnvPointer(const std::map<std::string, std::string>& envMap);
char** getArgvPointer(const std::map<std::string, std::string>& envMap);


#endif