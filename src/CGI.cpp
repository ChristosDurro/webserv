/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 10:29:50 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/15 12:19:10 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CGI.hpp"

HttpResponse processCgiReq(HttpRequest req, ClientSocket &client, ServerConfig &server)
{

	try
	{
		std::map<std::string, std::string> envs = getCgiEnv(req, client, server);

		// Check file exists and is executable
		std::ifstream file(envs["PATH_TRANSLATED"].c_str());
		if (!file.good())
		{
			return createHttpResponse(404, server.getErrorPage(404));
		}

		return createHttpResponse(executeCgi(envs, req.body, server));
	}
	catch (const std::exception &e)
	{
		std::cout << "CGI exception: " << e.what();
		return createHttpResponse(500, server.getErrorPage(500));
	}
}

std::map<std::string, std::string> getCgiEnv(HttpRequest req, ClientSocket &client, ServerConfig &server)
{

	std::map<std::string, std::string> cgiEnvs;

	(void)client;

	cgiEnvs["CONTENT_LENGTH"] = req.body.size();
	cgiEnvs["CONTENT_TYPE"] = req.headers["Content-Type"];
	cgiEnvs["GATEWAY_INTERFACE"] = "CGI/1.1";
	cgiEnvs["PATH_INFO"] = req.uri;
	cgiEnvs["PATH_TRANSLATED"] = fullPath("public/scripts/", getScriptName(req.uri));
	cgiEnvs["QUERY_STRING"] = getQueryString(req.uri);
	// cgiEnvs["REMOTE_ADDR"] = toIpString(client.get);
	cgiEnvs["REQUEST_METHOD"] = req.method;
	cgiEnvs["SCRIPT_NAME"] = getScriptName(req.uri);
	cgiEnvs["SERVER_NAME"] = server.getServerName();
	cgiEnvs["SERVER_PORT"] = req.headers["Host"].substr(req.headers["Host"].find(":") + 1);
	cgiEnvs["SERVER_PROTOCOL"] = "HTTP/1.1";

	return cgiEnvs;
}

std::string getScriptName(const std::string &uri)
{
	if (uri.find("/script/") == std::string::npos)
	{
		throw std::runtime_error("no " + std::string("/script/") + " at the start");
	}

	size_t end = uri.find_first_of("/", 1);
	return uri.substr(end);
}

std::string getQueryString(std::string &uri)
{
	size_t sepPos = uri.find_first_of("?");
	return sepPos == std::string::npos ? "" : uri.substr(sepPos + 1);
}

std::string executeCgi(std::map<std::string, std::string> envs, std::string body, ServerConfig server)
{
	int pipeIn[2];
	int pipeOut[2];
	pid_t pid;
	time_t cgiStartTime = time(0);

	if (pipe(pipeIn) == -1 || pipe(pipeOut) == -1)
	{
		throw std::runtime_error("pipe() failed");
	}

	if ((pid = fork()) == -1)
	{
		throw std::runtime_error("fork() failed");
	}

	std::string result = "";
	if (pid == 0)
	{
		try
		{
			// Memory allocated inside child process, no need to free
			char **argv = getArgvPointer(envs);
			char **env = getEnvPointer(envs);

			// Change to the correct directory for relative path file access
			if (chdir(fullPath("public", "/scripts/").c_str()) == -1)
			{
				throw std::runtime_error("chdir() failed");
			}

			dup2(pipeIn[0], 0), dup2(pipeOut[1], 1);
			close(pipeIn[1]), close(pipeOut[0]);

			if (execve(argv[0], argv, env) == -1)
			{

				std::cerr << "execve() failed: " << strerror(errno) << std::endl;
				exit(1);
			}
		}
		catch (const std::exception &e)
		{
			std::cout << RED << "CGI child process exception: " << DEFAULT << e.what();
			exit(1);
		}
	}
	else
	{
		int status;

		// Pipe stdin
		close(pipeOut[1]), close(pipeIn[0]);
		write(pipeIn[1], body.c_str(), body.size());
		close(pipeIn[1]);

		// Pipe stdout
		char buffer[4096];
		ssize_t bytesRead;
		while ((bytesRead = read(pipeOut[0], buffer, 4096)) > 0)
		{
			result += std::string(buffer, bytesRead);
			if (time(0) - cgiStartTime >= CGI_TIMEOUT)
			{
				close(pipeOut[0]);
				std::cout << RED << time(0) - cgiStartTime << " seconds passed. CGI timeout reached!" << DEFAULT << std::endl;
				HttpResponse res = createHttpResponse(408, server.getErrorPage(408));
				return res.body;
			}
		}
		close(pipeOut[0]);

		// std::cout << result << std::endl;
		waitpid(pid, &status, 0);
		if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0))
		{
			std::cerr << "child process failed: " << strerror(errno) << std::endl;
			throw std::runtime_error("child process error");
		}
	}
	return result;
}

char **getEnvPointer(const std::map<std::string, std::string> &envMap)
{
	char **envPointer = new char *[envMap.size() + 1];

	size_t i = 0;
	for (std::map<std::string, std::string>::const_iterator it = envMap.begin(); it != envMap.end(); it++)
	{
		std::string envString = it->first + "=" + it->second;
		envPointer[i] = new char[envString.size() + 1];
		std::strcpy(envPointer[i], envString.c_str());
		i++;
	}
	envPointer[envMap.size()] = NULL;
	return envPointer;
}

char **getArgvPointer(const std::map<std::string, std::string> &envMap)
{
	std::map<std::string, std::string>::const_iterator it = envMap.find("PATH_TRANSLATED");
	if (it == envMap.end())
	{
		throw std::runtime_error("couldn't find PATH_TRANSLATED");
	}

	std::string ext = getFileExtension(it->second);
	if (ext != ".php")
	{
		throw std::runtime_error("extension " + ext + " not supported");
	}

	char **argvPointer = new char *[3];
	std::string execPath = "/usr/bin/php";
	std::string scriptPath = it->second.substr(it->second.find_last_of("/") + 1);

	argvPointer[0] = new char[20];
	std::strcpy(argvPointer[0], execPath.c_str());
	argvPointer[1] = new char[it->second.size() + 1];
	std::strcpy(argvPointer[1], scriptPath.c_str());
	argvPointer[2] = NULL;
	return argvPointer;
}