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
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		servers[i]->startup();
		_addFd(_servers[i]->getSocket());
	}
}

void	Supervisor::execution( void )
{
	if (_servers.size() == 0)
		throw NoServerAdded();
	while (true)
	{
		if (poll(_fds.data(), _fds.size(), 0) == -1)
			throw FailedPoll();
		for (size_t i = 0; i < _fds.size(); ++i)
		{
			if (_fds[i].revents == 0)
				continue ;
			if (!(_fds[i].revents & POLLIN))
				continue ;
			int fd = _fds[i].fd;
			if (i < _servers.size())
			{
				if (_fds.size() >= FDS_SIZE)
					throw TooManyConnexions();
				Client * client = new Client(fd);
				_clients.push_back(client);
				_addFd(client->getSocket());
				printf("New client [%d] connexion.\n", client->getSocket());
			}
			else
			{
				char buffer[BUFFER_SIZE];
				int rc = recv(fd, buffer, sizeof(buffer), 0);
				if (rc == -1)
					continue ;
				else if (rc == 0)
				{
					printf("Client [%d] s'est déconnecté.\n", fd);
					if (!_removeClient(fd))
						throw SupNoClient();
					_fds[i] = _fds[_fds.size() - 1];
					continue ;
				}
				else
				{
					std::string response =
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: text/plain\r\n"
					"Content-Length: 13\r\n"
					"\r\n"
					"Hello, world!";
					if (send(fd, response.c_str(), response.size(), 0) == -1)
						throw FailedSend();
				}
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

bool	Supervisor::_removeClient( int fd )
{
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getSocket() == fd)
		{
			delete *it;
			_clients.erase(it);
			for (std::vector<pollfd>::iterator pit = _fds.begin(); pit != _fds.end(); ++pit)
				if (pit->fd == fd)
					_fds.erase(pit);
			return true;
		}
	}
	return false;
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
