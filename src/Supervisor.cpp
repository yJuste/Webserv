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
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		servers[i]->startup();
		_addFd(_servers[i]->getSocket());
	}
	_size = _fds.size();
}

void	Supervisor::execution( void )
{
	if (_servers.size() == 0)
		throw NoServerAdded();
	Print::header("DEBUG INFO", APPLE_GREEN);
	while (true)
	{
		if (poll(_fds.data(), _size, 0) == -1)
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
					throw TooManyConnexions();
				Client * client = new Client(fd, _servers[i]);
				_clients.push_back(client);
				_addFd(client->getSocket());
				++_size;
				Print::debug(APPLE_GREEN, client->getSocket(), "Logged in.");
			}
			else
			{
				size_t idx = i - _server_size;
				Client * client = _clients[idx];
				char buffer[BUFFER_SIZE] = {0};
				int rc = recv(fd, buffer, sizeof(buffer), 0);
				if (rc == -1)
					continue ;
				else if (rc == 0)
				{
					Print::debug(RED, client->getSocket(), "Logged out.");
					delete client;
					_clients.erase(_clients.begin() + idx);
					_fds[i] = _fds[_size - 1];
					--_size;
					continue ;
				}
				client->read(buffer, rc);
				client->write();
			}
		}
	}
}

// Private Methods

void	Supervisor::_addFd( int socket )
{
	struct pollfd pfd;
	pfd.fd = socket;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_fds.push_back(pfd);
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
