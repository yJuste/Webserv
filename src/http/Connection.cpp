/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:06:26 by layang            #+#    #+#             */
/*   Updated: 2025/09/02 20:26:10 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(int fd, Server* server) : _fd(fd), _server(server)
{}

int Connection::getFd() const
{
	return _fd;
}

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
    std::cout << "Request received: " << std::endl; ///
    std::cout << buffer << std::endl;   ///
    _readBuffer.append(buffer, n);
    _request.parseRequest(_readBuffer);

    if (_request.isComplete()) {
        HttpResponse response;
        //response.setLocations(_server->getLocations());
        response.buildResponse(_request, _server);    //
        _writebBuffer = response.toString();
        _readBuffer.clear();
        _request.reset();
    }
    return true;
}

/* void Connection::writeToClient()
{
    if (_writebBuffer.empty())
        return;

    int n = send(_fd, _writebBuffer.data(), _writebBuffer.size(), 0);
    if (n < 0)
    {
        _closed = true;
        return;
    }

    _writebBuffer.erase(0, n);
} */

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