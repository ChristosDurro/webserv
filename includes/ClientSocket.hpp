/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 12:15:37 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/18 11:40:14 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "ServerConfig.hpp"
#include "definitions.hpp"

class ClientSocket
{
private:
	bool _done;
	bool _disconnection;
	bool _isComplete;
	bool _requestParsed;
	bool _requestIsReady;
	bool _responseIsReady;
	bool _chunk;

	int _contentLength;
	int _bytesReceived;
	int _fd;

	time_t _timeStarted;

	std::string _requestStr;
	std::string _body;
	std::string _chunkBuffer;
	std::queue<std::string> _responseQueue;
	ServerConfig _config;
	HttpRequest _request;
	HttpResponse _response;

public:
	ClientSocket(int socketFD);
	~ClientSocket();

	int getContentLength() const;
	int getFD() const;
	int getBytesReceived() const;

	std::string getRequestStr() const;

	void reset();
	void socketSetup();
	void sendResponse();
	void setResponse(const std::string &response);
	void appendToRequest(const std::string &data);
	void setComplete(bool complete);
	void setContentLength(int length);
	void parseHttpHeaders(std::string &buffer);
	void parseHttpBody(std::string &buffer);
	void setBytesReceived(int received);
	void setDisconnect(bool value);
	void setResponse(HttpResponse res);

	bool isDone() const;
	bool getDisconnection() const;
	bool hasResponseToSend() const;
	bool isComplete() const;
	bool getRequestParsed() const;
	bool getRequestIsReady() const;
	bool hasCommunicationTimeout();

	HttpRequest getRequest() const;
	HttpResponse getResponse() const;
};

#endif