/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poller.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 09:22:57 by layang            #+#    #+#             */
/*   Updated: 2025/09/24 15:46:52 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Poller.hpp"

Poller::~Poller() {
    for (std::map<int, Connection*>::iterator it = _connections.begin(); it != _connections.end(); ++it) {
        delete it->second;
    }
    _connections.clear();
}

void Poller::addFd(int fd, short events, Connection *conn)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	pfd.revents = 0;
	_fds.push_back(pfd);
	_connections[fd] = conn;
}

void Poller::removeFd(int fd)
{
	for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_fds.erase(it);
			break;			
		}
	}
	std::map<int, Connection*>::iterator it = _connections.find(fd);
    if (it != _connections.end())
    {
        delete it->second;
        _connections.erase(it);
    }

    ::close(fd);
}

void Poller::modifyFd(int fd, short events) {
    for (size_t i = 0; i < _fds.size(); i++) {
        if (_fds[i].fd == fd) {
            _fds[i].events = events;
            return;
        }
    }
}

void Poller::run(const std::vector<Server*>& servers, SessionManager& sessionManager)
{
    while (true)
    {
        int ret = poll(_fds.data(), _fds.size(), -1);
        if (ret < 0)
            throw std::runtime_error("poll failed");
        
        sessionManager.cleanupExpired();
            
        for (size_t i = 0; i < _fds.size(); i++)
        {
            int fd = _fds[i].fd;
            short revents = _fds[i].revents;

            if (revents == 0)
                continue;
            if (revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                if (_connections[fd])
                    removeFd(fd);
                continue;
            }
            if (revents & POLLIN)
            {
                if (_connections[fd] == NULL)
                {
                    int clientFd = accept(fd, NULL, NULL);
                    if (clientFd >= 0)
                    {
                        Server* serverOwner = NULL;
                        for (size_t s = 0; s < servers.size(); ++s)
                        {
                            if (servers[s]->getSocket() == fd)
                            {
                                serverOwner = servers[s];
                                break;
                            }
                        }
                        if (serverOwner)
                        {
                            Connection* conn = new Connection(clientFd, serverOwner, sessionManager);
                            addFd(clientFd, POLLIN, conn);
                        }
                    }
                }
                else
                {
                    Connection* conn = _connections[fd];
                    bool alive = conn->readFromClient();
                    if (!alive)
                        continue;
                    if (conn->hasDataToWrite())
                        modifyFd(fd, POLLIN | POLLOUT);
                    alive = _connections[fd]->writeToClient();
                    if (!alive)
                    {
                        removeFd(fd);
                        continue;
                    }
                    if (!_connections[fd]->hasDataToWrite())
                    {
                        modifyFd(fd, POLLIN);
                        conn->resetRequest();
                    }
                }
            }
        }
    }
}
