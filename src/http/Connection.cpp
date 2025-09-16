/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:06:26 by layang            #+#    #+#             */
/*   Updated: 2025/09/16 18:46:38 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(int fd, Server* server) : _fd(fd), _server(server)
{}

int Connection::getFd() const
{
	return _fd;
}

void Connection::resetRequest()
{
    _request.reset();
}

Connection::~Connection() {}

bool Connection::hasDataToWrite() const {
    return !_writebBuffer.empty();
}

bool Connection::readFromClient()
{
    char buffer[READ_SIZE];
    int n = recv(_fd, buffer, sizeof(buffer), 0);
    if (n == 0)
        return false;
    else if (n == -1)
        return true;
    _readBuffer.append(buffer, n);
    _request.parseRequest(_readBuffer);

    if (_request.isComplete()) {
		if (!_request.hasPrinted())
			_request.setPrinted(true);
		std::cout << "\r\n\r\n" << std::endl;
        std::cout << "      ----****  new request ****----: " << std::endl;
        std::cout << "Request complete: "
                  << _request.getMethod() << " "
                  << _request.getPath();
        if (!_request.getQueryString().empty())
            std::cout << "?" << _request.getQueryString();

        std::cout << " " << _request.getHttpVersion() << std::endl;
        const std::map<std::string, std::string>& headers = _request.getHeaders();
        for (std::map<std::string, std::string>::const_iterator it = headers.begin();
             it != headers.end(); ++it)
        {
            std::cout << it->first << ": " << it->second << std::endl;
        }
        if (!_request.getRequestBody().empty()) {
            std::string body = _request.getRequestBody();
            std::string firstLine;

            size_t pos = body.find('\n');
            if (pos != std::string::npos)
                firstLine = body.substr(0, pos);
            else
                firstLine = body;

            std::cout << "---Request Body first line: " << firstLine << std::endl;
        }
        HttpResponse response;
        response.buildResponse(_request, _server);
        _writebBuffer = response.toString(_request);
        _readBuffer.clear();
        return true;
    }
    return false;
}

bool Connection::writeToClient()
{
    if (_writebBuffer.empty())
        return true;

    int n = send(_fd, _writebBuffer.data(), _writebBuffer.size(), 0);
    if (n <= 0)
        return false;
    
    _writebBuffer.erase(0, n);
    return true;
}
