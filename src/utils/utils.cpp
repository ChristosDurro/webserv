/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttaneski <ttaneski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:46:53 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/14 12:43:30 by ttaneski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"

std::vector<std::string> split(const std::string &str, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(str);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

size_t firstWordLen(std::string str)
{
	int i = 0;

	while (str[i] && ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') || str[i] == '_'))
		i++;
	// for error handling
	if (str[i] != ' ')
		return -1;
	return i;
}

bool endsWith(std::string str1, std::string str2)
{
	if (str1.length() < str2.length())
		return false;
	if (str1.substr(str1.length() - str2.length()) == str2)
		return true;
	return false;
}

void trimChars(std::string &input, const std::string &charsToTrim)
{
	size_t start = input.find_first_not_of(charsToTrim);
	size_t end = input.find_last_not_of(charsToTrim);

	if (start == std::string::npos || end == std::string::npos)
		return;
	input = input.substr(start, end - start + 1);
}

void trimSpaces(std::string &str)
{
	int i = 0;
	while (str[i] && (str[i] == ' ' || str[i] == '\t'))
		i++;
	str = str.substr(i, str.length() - i);

	if (str[i] == '\0')
	{
		return;
	}
	i = str.length() - 1;
	while (str[i] && (str[i] == ' ' || str[i] == '\t'))
		i--;
	str = str.substr(0, i + 1);
}

std::string extractPath(std::string &str, size_t startPos, size_t len)
{
	// std::cout << "str: " << str << "| Start pos: " << startPos << "| Len: " << len << std::endl;
	str.erase(startPos, len);
	// std::cout << "Erase: " << str << std::endl;
	str.resize(str.size() - 1);
	// std::cout << "Resize: " << str << std::endl;
	trimSpaces(str);
	// std::cout << "Trimmed: " << str << std::endl;
	return str;
}

std::string extractLocationConent(std::string &str, std::ifstream &config_file)
{
	size_t start = 0;
	size_t end = 0;
	std::string new_str;

	start = str.find_first_of("{") + 1;
	new_str = str.substr(start, end - start);
	std::getline(config_file, str, '}');
	return (new_str + str);
}

int countWords(std::string str)
{
	int count = 0;

	int i = 0;
	while (str[i])
	{
		// std::cout << "str[i]: " << str[i] << std::endl;
		if ((((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z')) && i != 0 && str[i - 1] == ' '))
			count++;
		i++;
	}
	return count;
}

std::string extractString(std::string str, size_t len)
{
	str.erase(0, len);
	if (str.find(";") != std::string::npos)
		str.erase(str.find(";"), 1);
	trimSpaces(str);
	return str;
}

int extractInt(std::string str, size_t len)
{
	int num;

	str.erase(0, len);
	if (str.find(";") != std::string::npos)
		str.erase(str.find(";"), 1);
	trimSpaces(str);

	std::istringstream iss(str);
	iss >> num;

	return num;
}


std::string getMimeType(std::string ext)
{
	if (ext == ".txt") return "text/plain";
	if (ext == ".htm" || ext == ".html") return "text/html";
	if (ext == ".php") return "application/x-httpd-php";

	return "text/plain";
}

/* --------------------------------------------------------------------------------------------- */
bool isAllDigit(const std::string& str)
{
	for (size_t i = 0; i < str.length(); i++) {
		if (!std::isdigit(str[i])) return false;
	}
	return true;
}

/* ---------------------------------------------------------------------------------------------- */
std::string toString(int value)
{
	std::stringstream ss;
	ss << value;

	// Unlikely to happen, bad alloc only
	if (ss.fail()) return "";

	return ss.str();
}

/* ---------------------------------------------------------------------------------------------- */
std::string toIpString(in_addr_t ip)
{
	std::ostringstream oss;
	unsigned char* bytes = reinterpret_cast<unsigned char*>(&ip);

	oss << static_cast<int>(bytes[0]) << '.'
		<< static_cast<int>(bytes[1]) << '.'
		<< static_cast<int>(bytes[2]) << '.'
		<< static_cast<int>(bytes[3]);

	return oss.str();
}


/* ---------------------------------------------------------------------------------------------- */
std::string getFileContent(const std::string& path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open()) {
		throw std::runtime_error("failed to open file " + path);
	}

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

/* ---------------------------------------------------------------------------------------------- */
std::string getFileExtension(const std::string& path)
{
	size_t pos = path.find_last_of('.');

	if (pos != std::string::npos && pos != 0) return path.substr(pos);
	return "";
}

/* ---------------------------------------------------------------------------------------------- */
std::string fullPath(std::string root, std::string path)
{

	try {
		// Remove '/' from root and path
		root = root.at(root.size() - 1) == '/' ? root.substr(0, root.size() - 1) : root;
		path = path.at(0) == '/' ? path.substr(1) : path;
	}
	catch (const std::exception& e) {
		// Protect against if strings are empty or index out of bound
		std::cout << "FullPath exception: " << e.what();
		return "";
	}
	return root + "/" + path;
}

/* ---------------------------------------------------------------------------------------------- */
std::string getBoundary(const HttpRequest& req)
{
	std::map<std::string, std::string>::const_iterator it;
	it = req.headers.find("Content-Type");
	if (it == req.headers.end()) {
		throw std::runtime_error("couldn't find Content-Type header");
	}

	size_t pos = it->second.find("boundary=");
	if (pos == std::string::npos) {
		throw std::runtime_error("couldn't find 'boundary='");
	}

	return "--" + it->second.substr(pos + 9);
}

/* ---------------------------------------------------------------------------------------------- */
int toInt(const std::string& str)
{
	std::istringstream iss(str);
	int result;
	char remainingChar;

	// Check if there are leftover characters
	if (!(iss >> result) || (iss.get(remainingChar))) {
		throw std::runtime_error("convert " + str + " toInt fails");
	}

	// Check fail and edge cases
	if (result == 0 && str != "0" && str != "+0" && str != "-0") {
		throw std::runtime_error("convert " + str + " toInt fails");
	}
	if (result == INT_MAX && str != "2147483647" && str != "+2147483647") {
		throw std::runtime_error("convert " + str + " toInt fails");
	}
	if (result == INT_MIN && str != "-2147483648") {
		throw std::runtime_error("convert " + str + " toInt fails");
	}

	return result;
}


// teo
//  Function to generate a directory listing in HTML
std::string generateDirectoryListing(const std::string &path, const std::string &uri) {
    std::cout << "path in generate: " << path << std::endl;
    std::stringstream ss;
    ss << "<html><head><title>Directory listing for " << path << "</title></head>";
    ss << "<body><h1>Directory listing for " << path << "</h1><ul>";

    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(path.c_str())) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;
            if (name != "." && name != "..") {
                std::string itemPath = uri + (uri.size() > 0 && uri[uri.size() - 1] == '/' ? "" : "/") + name;
                ss << "<li><a href=\"" << itemPath << "\">" << name << "</a></li>";
            }
        }
        closedir(dir);
    } else {
        perror("opendir");
    }
    ss << "</ul></body></html>";
    return ss.str();
}

