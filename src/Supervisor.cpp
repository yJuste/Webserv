// ************************************************************************** //
//                                                                            //
//                Supervisor.cpp                                              //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Supervisor.hpp"

Supervisor::Supervisor() : _smanager(NULL), _size(0), _server_size(0) {}
Supervisor::~Supervisor() { _clean(); }

Supervisor::Supervisor( const std::vector<Server *> & servers ) : _smanager(NULL), _size(0), _server_size(0) { hold(servers); }

Supervisor::Supervisor( const Supervisor & s ) { *this = s; }

Supervisor	& Supervisor::operator = ( const Supervisor & s )
{
	if (this != &s)
	{
		for (size_t i = 0; i < FDS_SIZE; ++i)
			_fds[i] = s._fds[i];
		_servers = s._servers;
		_clients = s._clients;
		_smanager = s._smanager;
		_size = s._size;
		_server_size = s._server_size;
	}
	return *this;
}

// Methods

void	Supervisor::hold( const std::vector<Server *> & servers )
{
	if (!_servers.empty())
		return ;
	_servers = servers;
	_server_size = servers.size();
	_size = _server_size;
	if (_server_size != 0)
		Print::header("DEBUG INFO", APPLE_GREEN);
	for (size_t i = 0; i < _server_size; ++i)
	{
		servers[i]->startup();
		_fds[i].fd = _servers[i]->getSocket();
		_fds[i].events = POLLIN;
		_fds[i].revents = 0;
	}
	_fds[_size].fd = STDIN_FILENO;
	_fds[_size].events = POLLIN;
	_fds[_size].revents = 0;
	++_size;
	_smanager = new SessionManager();
}

void	Supervisor::execution( void )
{
	if (_server_size == 0)
		throw NoServerAdded();

	time_t lastHelp = std::time(0);
	bool last_print = true;
	_clock(last_print, lastHelp);
	while (true)
	{
		_clock(last_print, lastHelp);
		int ret = poll(_fds, _size, 3000);
		if (ret <= -1)
			throw FailedPoll();
		else if (ret == 0)
			continue ;
		for (size_t i = 0; i < _size; ++i)
		{
			if (_fds[i].revents == 0)
				continue ;
			if (!(_fds[i].revents & POLLIN))
				continue ;

			int fd = _fds[i].fd;
			if (fd == STDIN_FILENO)
			{
				std::string input;
				std::getline(std::cin, input);
				if (input == "help")
					last_print = true;
				else if (input == "config")
					for (size_t j = 0; j < _server_size; ++j)
						_servers[j]->myConfig();
				else if (input == "quit" || input == "stop")
					return (void)(std::cout << std::string(APPLE_GREEN) << "Quit properly." << std::string(RESET) << std::endl);
				continue ;
			}
			if (i < _server_size)
			{
				if (_size >= FDS_SIZE)
				{
					size_t first_client = _server_size + 1;
					int old_fd = _fds[first_client].fd;
					Client * old_client = _getClient(old_fd);
					if (old_client)
						_supClient(old_fd);
					for (size_t j = first_client; j < _size - 1; ++j)
						_fds[j] = _fds[j + 1];
					--_size;
				}
				Client * client = new Client(fd, _servers[i], _smanager);
				_clients.push_back(client);
				_fds[_size].fd = client->getSocket();
				_fds[_size].events = POLLIN;
				_fds[_size].revents = 0;
				++_size;
				continue ;
			}
			Client * client = _getClient(fd);
			if (!client)
				continue ;
			char buffer[BUFFER_SIZE];
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
			client->read(std::string(buffer, rc));
			client->write();
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

void	Supervisor::_clock( bool & last_print, time_t & lastHelp )
{
	time_t now = std::time(0);
	if (now - lastHelp >= 1)
	{
		_smanager->hasExpired(now);
		if (last_print)
		{
			std::cout << "    | " << std::string(APPLE_GREEN) << "Helping Page for Webserv" << std::string(RESET) << ": ( write in the terminal )" << std::endl;
			std::cout << "    |\t- [" << std::string(APPLE_GREEN) << "help" << std::string(RESET) << "]       Show this page." << std::endl;
			std::cout << "    |\t- [" << std::string(APPLE_GREEN) << "config" << std::string(RESET) << "]     Print the server's configurations." << std::endl;
			std::cout << "    |\t- [" << std::string(APPLE_GREEN) << "quit" << std::string(RESET) << "]       Quit the server." << std::endl;
			lastHelp = now;
			last_print = false;
		}
		else
			lastHelp = now;
	}
}

void	Supervisor::_clean( void )
{
	for (size_t i = 0; i < _server_size; ++i)
		delete _servers[i];
	_servers.clear();
	if (!_clients.empty())
	{
		for (size_t i = 0; i < _clients.size(); ++i)
			delete _clients[i];
		_clients.clear();
	}
	delete _smanager;
}
