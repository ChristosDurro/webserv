/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 13:44:03 by cdurro            #+#    #+#             */
/*   Updated: 2024/02/26 13:45:02 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <poll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <string.h>
#include <queue>
#include <dirent.h>
#include <signal.h>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include "definitions.hpp"
#include <climits>
#include "sys/stat.h"
#include <algorithm>
#include <sys/time.h>

extern bool g_signalPressed;

bool isAllDigit(const std::string& str);
std::string toString(int value);
std::string toIpString(in_addr_t ip);
std::string getFileContent(const std::string& path);
std::string getMimeType(std::string ext);
std::string getFileExtension(const std::string& path);
std::string fullPath(std::string root, std::string path);
std::string getBoundary(const HttpRequest& req);
int toInt(const std::string& str);


bool endsWith(std::string str1, std::string str2);

int countWords(std::string str);
int extractInt(std::string str, size_t len);

size_t firstWordLen(std::string str);

std::string extractString(std::string str, size_t len);
std::string extractPath(std::string &str, size_t startPos, size_t len);
std::string extractLocationConent(std::string &str, std::ifstream &config_file);
std::string generateDirectoryListing(const std::string &path, const std::string &uri);


void trimSpaces(std::string &str);
void trimChars(std::string &input, const std::string &charsToTrim);

std::vector<std::string> split(const std::string &str, char delimiter);


#endif