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
		int status;
		while (waitpid(-1, &status, WNOHANG) > 0)
			;
		for (size_t i = 0; i < _size; ++i)
		{
			short revents = _fds[i].revents;
			if (_fds[i].revents == 0)
				continue ;
			int fd = _fds[i].fd;
			if (fd == STDIN_FILENO && (revents & POLLIN))
			{
				if (_supervise_stdin(last_print))
					return ;
			}
			else if (i < _server_size && (revents & POLLIN))
				_new_client(fd, i);
			Client * client = _getClient(fd);
			if (!client)
				continue ;
			else if (revents & POLLIN)
				_reading(client, fd, i);
			else if (revents & POLLOUT)
				_writing(client, fd, i);
		}
	}
}

// Private Methods

/*
 *	STDIN
 */

bool	Supervisor::_supervise_stdin( bool & last_print )
{
	std::string input;
	std::getline(std::cin, input);
	if (input == "help")
		last_print = true;
	else if (input == "config")
		for (size_t j = 0; j < _server_size; ++j)
			_servers[j]->myConfig();
	else if (input == "quit" || input == "stop")
		return std::cout << std::string(APPLE_GREEN) << "Quit properly." << std::string(RESET) << std::endl, true;
	return false;
}

/*
 *	NEW_CLIENT
 */

void	Supervisor::_new_client( int fd, int idx )
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
	Client * client = new Client(fd, _servers[idx], _smanager);
	_clients.push_back(client);
	_fds[_size].fd = client->getSocket();
	_fds[_size].events = POLLIN;
	_fds[_size].revents = 0;
	++_size;
}

/*
 *	READING
 */

void	Supervisor::_reading( Client * client, int fd, int idx )
{
	char buffer[BUFFER_SIZE];
	if (fd == client->getSocket())
	{
		int rc = client->receive(fd, buffer, sizeof(buffer));
		if (rc <= 0)
		{
			_supClient(fd);
			_fds[idx] = _fds[_size - 1];
			--_size;
			return ;
		}
		if (client->retrieve(std::string(buffer, rc)) == 1)
		{
			_fds[_size].fd = client->getSv();
			_fds[_size].events = POLLIN;
			_fds[_size].revents = 0;
			++_size;
			return ;
		}
		client->setOriginal(client->wbuf().size());
		_fds[idx].events = POLLIN | POLLOUT;
	}
	else if (fd == client->getSv())
	{
		int rc = client->receive(fd, buffer, sizeof(buffer));
		if (rc <= 0)
		{
			close(fd);
			_fds[idx] = _fds[_size - 1];
			--_size;
			finish_cgi(client->wbuf());
			client->getRequest()->reset();
			client->setSv(-1);
			for (size_t j = 0; j < _size; ++j)
			{
				if (_fds[j].fd == client->getSocket())
				{
					_fds[j].events = POLLIN | POLLOUT;
					break;
				}
			}
			client->setOriginal(client->wbuf().size());
		}
		client->wbuf().append(buffer, rc);
	}
}

/*
 *	WRITING
 */

void	Supervisor::_writing( Client * client, int fd, int idx )
{
	ssize_t n = client->write();
	if (n <= 0)
	{
		_supClient(fd);
		_fds[idx] = _fds[_size - 1];
		--_size;
		return ;
	}
	if (client->wbuf().empty())
	{
		_fds[idx].events = POLLIN;
		client->sent();
	}
}

/*
 *	UTILS
 */

Client * Supervisor::_getClient( int fd )
{
	for (std::vector<Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
		if ((*it)->getSocket() == fd || (*it)->getSv() == fd)
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
