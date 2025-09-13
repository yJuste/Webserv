// ************************************************************************** //
//                                                                            //
//                Supervisor.cpp                                              //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Supervisor.hpp"

Supervisor::Supervisor() {}
Supervisor::~Supervisor() { _clean(); }

Supervisor::Supervisor( const std::vector<Server *> & servers ) { hold(servers); }

// Methods

void	Supervisor::hold( const std::vector<Server *> & servers )
{
	if (!_servers.empty())
		return ;
	_servers = servers;
	_server_size = servers.size();
	_size = _server_size;
	for (size_t i = 0; i < _server_size; ++i)
	{
		servers[i]->startup();
		_fds[i].fd = _servers[i]->getSocket();
		_fds[i].events = POLLIN;
		_fds[i].revents = 0;
	}
}

void	Supervisor::execution( void )
{
	if (_server_size == 0)
		throw NoServerAdded();
	Print::header("DEBUG INFO", APPLE_GREEN);
	while (true)
	{
		if (poll(_fds, _size, 0) == -1)
			throw FailedPoll();
		for (size_t i = 0; i < _size; ++i)
		{
			if (_fds[i].revents == 0)
				continue ;
			if (!(_fds[i].revents & POLLIN))
				continue ;
			int fd = _fds[i].fd;
			if (i < _server_size)
			{
				if (_size >= FDS_SIZE)
				{
					Print::debug(RED, "error", "Too many connexions on a server.");
					continue ;
				}
				Client * client = new Client(fd, _servers[i]);
				_clients.push_back(client);
				_fds[_size].fd = client->getSocket();
				_fds[_size].events = POLLIN;
				_fds[_size].revents = 0;
				++_size;
			}
			else
			{
				Client * client = _getClient(fd);
				char buffer[BUFFER_SIZE] = {0};
				int rc = recv(fd, buffer, sizeof(buffer), 0);
				if (rc == -1)
					continue ;
				else if (rc == 0)
				{
					_supClient(fd);
					_fds[i] = _fds[_size - 1];
					--_size;
					continue ;
				}
				client->read(buffer);
				client->write();
			}
		}
	}
}

// Private Methods

Client * Supervisor::_getClient( int fd )
{
	for (std::vector<Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
		if ((*it)->getSocket() == fd)
			return *it;
	return NULL;
}

Client * Supervisor::_supClient( int fd )
{
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getSocket() == fd)
		{
			delete *it;
			_clients.erase(it);
			return *it;
		}
	}
	return NULL;
}

void	Supervisor::_clean( void )
{
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		delete _servers[i];
		_servers[i] = NULL;
	}
	_servers.clear();
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		delete _clients[i];
		_servers[i] = NULL;
	}
	_clients.clear();
}
