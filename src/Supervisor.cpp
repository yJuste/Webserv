// ************************************************************************** //
//                                                                            //
//                Supervisor.cpp                                              //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Supervisor.hpp"
<<<<<<< HEAD
# include "Exceptions.hpp"
=======
>>>>>>> main

Supervisor::Supervisor() : _size(0), _server_size(0) { std::memset(_fds, 0, sizeof(_fds)); }
Supervisor::~Supervisor() { _clean(); }

Supervisor::Supervisor( const std::vector<Server *> & servers ) : _size(0), _server_size(0)
{
	std::memset(_fds, 0, sizeof(_fds));
	hold(servers);
}

// Methods

void	Supervisor::hold( const std::vector<Server *> & servers )
{
	if (_server_size)
		return ;
	_servers = servers;
	_server_size = _servers.size();
	_size = _server_size;
	for (size_t i = 0; i < _server_size; ++i)
	{
		servers[i]->startup();
		_fds[i].fd = _servers[i]->getSocket();
		_fds[i].events = POLLIN;
	}
}

void	Supervisor::execution( void )
{
	if (_server_size == 0)
		throw NoServerAdded();
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
					throw TooManyConnexions();
				Client * client = new Client(fd);
				addClient(client);
				_fds[_size].fd = client->getSocket();
				_fds[_size].events = POLLIN;
				++_size;
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
					if (!_supClient(fd))
						throw SupNoClient();
					_fds[i] = _fds[_size - 1];
					close(fd);
					--_size;
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

// Getter

size_t Supervisor::getSize() const { return _size; }

// Setter

void Supervisor::addClient( Client * client ) { _clients.push_back(client); }

// Private Methods

bool	Supervisor::_find( const std::vector<Server *> & servers, int fd )
{
	for (size_t i = 0; i < servers.size(); ++i)
		if (servers[i]->getSocket() == fd)
			return true;
	return false;
}

bool	Supervisor::_supClient( int fd )
{
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getSocket() == fd)
		{
			delete *it;
			_clients.erase(it);
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
