// ************************************************************************** //
//                                                                            //
//                Supervisor.cpp                                              //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Supervisor.hpp"
# include "Exceptions.hpp"

Supervisor::Supervisor() : _size(0) { std::memset(_fds, 0, sizeof(_fds)); }
Supervisor::~Supervisor() { _clean(); }

Supervisor::Supervisor( const std::vector<Server *> & servers ) : _size(servers.size()), _servers(servers)
{
	std::memset(_fds, 0, sizeof(_fds));

	for (size_t i = 0; i < _size; ++i)
	{
		_fds[i].fd = _servers[i]->getSocket();
		_fds[i].events = POLLIN;
	}
}

// Method

void	Supervisor::execution( void )
{
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
			if (_find(_servers, fd))
			{
				if (_size >= FDS_SIZE)
					throw TooManyConnexions();
				Client * client = new Client(fd);
				addClient(client);
				_fds[_size].fd = client->getSocket();
				_fds[_size].events = POLLIN;
				++_size;
				printf("New client connexion.\n");
			}
			else
			{
				char buffer[1024];
				int rc = recv(fd, buffer, sizeof(buffer), 0);
				if (rc <= 0)
				{
					printf("Client %d s'est déconnecté.\n", fd);
					_supClient(fd);
					close(fd);
					_fds[i] = _fds[_size - 1];
					--_size;
					--i;
					continue ;
				}
				else
				{
					std::string	response =
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: text/plain\r\n"
					"Content-Length: 13\r\n"
					"\r\n"
					"Hello, world!";
					send(fd, response.c_str(), response.size(), 0);
				}
			}
		}
	}
}

// Private Methods

bool	Supervisor::_find( const std::vector<Server *> & servers, int fd )
{
	for (size_t i = 0; i < servers.size(); ++i)
		if (servers[i]->getSocket() == fd)
			return true;
	return false;
}

void	Supervisor::_supClient( int fd )
{
	for (std::vector<Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getSocket() == fd)
		{
			delete *it;
			_clients.erase(it);
			return ;
		}
	}
}

void	Supervisor::_clean()
{
	for (size_t i = 0; i < _servers.size(); ++i)
		delete _servers[i];
	_servers.clear();
	for (size_t i = 0; i < _clients.size(); ++i)
		delete _clients[i];
	_clients.clear();
}

// Getters

size_t	Supervisor::getSize() const { return _size; }
struct pollfd	Supervisor::getFdX( int idx ) const { return _fds[idx]; }

// Setters

void	Supervisor::addClient( Client * client ) { _clients.push_back(client); }
