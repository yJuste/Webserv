/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poller.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 09:22:57 by layang            #+#    #+#             */
/*   Updated: 2025/09/03 20:07:51 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Poller.hpp"

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
	//_connections.erase(fd);
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

/* void Poller::run()
{
	while (true)
	{
		int ret = poll(_fds.data(), _fds.size(), -1);
		if (ret < 0)
			throw std::runtime_error("poll failed");
		for (size_t i = 0; i < _fds.size(); i++)
		{
			if (_fds[i].revents & POLLIN)
			{
				if (_connections[_fds[i].fd] == NULL) {
                    // Listening socket → accept new client
                    int clientFd = accept(_fds[i].fd, NULL, NULL);
                    if (clientFd >= 0) {
                        Connection* conn = new Connection(clientFd);
                        addFd(clientFd, POLLIN, conn);
                    }
                }
				else
				{
                    // Client socket → Connection handle read
					_connections[_fds[i].fd]->readFromClient();
				}
			}
			if ((_fds[i].revents & POLLOUT) && _connections[_fds[i].fd])
				_connections[_fds[i].fd]->writeToClient();
		}
		
	}
} */

void Poller::run(const std::vector<Server*>& servers)
{
    while (true)
    {
        int ret = poll(_fds.data(), _fds.size(), -1);
        if (ret < 0)
            throw std::runtime_error("poll failed");

        for (size_t i = 0; i < _fds.size(); i++)
        {
            int fd = _fds[i].fd;
            short revents = _fds[i].revents;

            if (revents == 0)
                continue;

            // ---- Error or hang-up events ----
            if (revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                if (_connections[fd])
                    removeFd(fd);
                continue;
            }

            // ---- Readable events ----
            if (revents & POLLIN)
            {
                if (_connections[fd] == NULL)
                {
                    // Listening socket → accept new client
                    int clientFd = accept(fd, NULL, NULL);
                    if (clientFd >= 0)
                    {
                        // Find which Server this listening fd belongs to
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
                            Connection* conn = new Connection(clientFd, serverOwner);
                            addFd(clientFd, POLLIN, conn);
                        }
                    }
                }
                else
                {
                    // Client socket → handle read
                    Connection* conn = _connections[fd];
                    bool alive = conn->readFromClient();
                    if (!alive)
                    {
                        //removeFd(fd);
                        //std::cout << "FD REMOVED+++++:    +++" << std::endl;
                        continue;
                    }
                    // If there is data to write → add POLLOUT
                    if (conn->hasDataToWrite())
                        modifyFd(fd, POLLIN | POLLOUT);
                    alive = _connections[fd]->writeToClient();
                    if (!alive)
                    {
                        removeFd(fd);
                        continue;
                    }
                    // If writing is finished → remove POLLOUT, keep only POLLIN
                    if (!_connections[fd]->hasDataToWrite())
                        modifyFd(fd, POLLIN);
                }
            }

            // ---- Writable events ----
            /*if (revents & POLLOUT)
            {
                if (_connections[fd])
                {
                    bool alive = _connections[fd]->writeToClient();
                    if (!alive)
                    {
                        removeFd(fd);
                        continue;
                    }
                    // If writing is finished → remove POLLOUT, keep only POLLIN
                    if (!_connections[fd]->hasDataToWrite())
                        modifyFd(fd, POLLIN);
                }
            }*/
        }
    }
}
