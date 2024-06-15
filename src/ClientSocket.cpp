/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdurro <cdurro@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 14:12:20 by cdurro            #+#    #+#             */
/*   Updated: 2024/06/15 10:21:24 by cdurro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ClientSocket.hpp"

ClientSocket::ClientSocket(int clientFD)
{
	_done = false;
	_disconnection = false;
	_fd = clientFD;
	_isComplete = false;
	_bytesReceived = 0;
	_contentLength = 0;
	_timeStarted = time(0);
	// _requestStr = "";
	// _body = "";
	_requestIsReady = true;
	_requestParsed = false;
	_chunk = false;
	// _chunkBuffer = "";
}

ClientSocket::~ClientSocket()
{
	close(_fd);
}

void ClientSocket::socketSetup()
{
}

void ClientSocket::appendToRequest(const std::string &data)
{
	_requestStr.append(data);
	_bytesReceived += data.size();
	if (_bytesReceived >= _contentLength)
		_isComplete = true;
}

void ClientSocket::setResponse(HttpResponse res)
{
	_response = res;
	_responseIsReady = true;
	_isComplete = true;
	_requestParsed = false;
	_responseQueue.push(toString(_response));
}

bool ClientSocket::hasCommunicationTimeout()
{
	if (time(0) - _timeStarted >= CLIENT_TIMEOUT)
		return true;
	return false;
}

bool ClientSocket::hasResponseToSend() const
{
	return _isComplete && !_responseQueue.empty();
}

void ClientSocket::setComplete(bool complete)
{
	_isComplete = complete;
}

bool ClientSocket::isComplete() const
{
	return _isComplete;
}

void ClientSocket::setContentLength(int length)
{
	_contentLength = length;
}

int ClientSocket::getContentLength() const
{
	return _contentLength;
}

void ClientSocket::setBytesReceived(int received)
{
	_bytesReceived = received;
}

int ClientSocket::getBytesReceived() const
{
	return _bytesReceived;
}

int ClientSocket::getFD() const
{
	return _fd;
}

void ClientSocket::parseHttpHeaders(std::string &buffer)
{
	// std::cout << "request now: " << buffer << std::endl;
	_requestStr.append(buffer);
	// std::cout << _requestStr << std::endl;
	if (_requestStr.find("\r\n\r\n") == std::string::npos)
		return;
	_request = parseHttpRequest(_requestStr);
	_requestParsed = true;

	buffer = buffer.substr(buffer.find("\r\n\r\n") + 4);

	std::map<std::string, std::string>::iterator it = _request.headers.find("Content-Length");
	if (it != _request.headers.end())
	{
		_contentLength = toInt(it->second);
		_requestIsReady = false;
	}

	it = _request.headers.find("Transfer-Encoding");
	if (it != _request.headers.end() && it->second == "chunked")
	{
		_chunk = true;
		_requestIsReady = false;
	}
}

void ClientSocket::parseHttpBody(std::string &buffer)
{
	if (_chunk)
	{
		// Handle chunked encoding
		std::cout << "GOT IN CHUNK" << std::endl;
		_chunkBuffer.append(buffer);
		while (true) {
			size_t pos = _chunkBuffer.find("\r\n");
			if (pos == std::string::npos) break;

			std::string sizeStr = _chunkBuffer.substr(0, pos);
			int size = strtol(sizeStr.c_str(), NULL, 16);
			if (size == 0) {
				_requestIsReady = true;
				_chunkBuffer.clear();
				break;
			}

			if (_chunkBuffer.size() < pos + 2 + size + 2) break;

			_request.body.append(_chunkBuffer.substr(pos + 2, size));
			_chunkBuffer.erase(0, pos + 2 + size + 2);
		}
	}
	else
	{
		_request.body.append(buffer);
		_bytesReceived += buffer.length();
		if (_bytesReceived >= _contentLength)
			_requestIsReady = true;
	}
}

bool ClientSocket::getRequestParsed() const
{
	return _requestParsed;
}

bool ClientSocket::getRequestIsReady() const
{
	return _requestIsReady;
}

void ClientSocket::reset()
{
	_contentLength = 0;
	_bytesReceived = 0;
	_requestIsReady = true;
	_requestParsed = false;
	_requestStr.clear();
	_chunkBuffer.clear();
	_chunk = false;
}

HttpRequest ClientSocket::getRequest() const
{
	return _request;
}
HttpResponse ClientSocket::getResponse() const
{
	return _response;
}

std::string ClientSocket::getRequestStr() const
{
	return _requestStr;
}

void ClientSocket::sendResponse()
{
	if (_responseQueue.empty())
		return;

	const std::string &response = _responseQueue.front();
	ssize_t bytesSent = send(_fd, response.c_str(), response.length(), 0);

	if (bytesSent <= 0)
	{
		perror("send");
		_disconnection = true;
		_done = true;
		return;
	}

	// If the entire response was sent, remove it from the queue
	if (static_cast<size_t>(bytesSent) == response.length())
	{
		std::cout << GREEN << "Client " << this->getFD() << " has responded with status code " << _response.statusCode << DEFAULT << std::endl;
		_responseQueue.pop();
	}
	else
	{
		// If not all data was sent, update the remaining part to be sent
		_responseQueue.front().erase(0, bytesSent);
	}

	// If the response queue is empty, mark the client as done
	if (_responseQueue.empty())
	{
		_done = true;
	}
}

void ClientSocket::setDisconnect(bool value)
{
	_disconnection = value;
}

bool ClientSocket::isDone() const
{
	return _done;
}

bool ClientSocket::getDisconnection() const
{
	return _disconnection;
}
