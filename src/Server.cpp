// ************************************************************************** //
//                                                                            //
//                Server.cpp                                                  //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

# include "Server.hpp"
# include "Exceptions.hpp"

Server::Server() {}
Server::~Server() {}

Server::Server( const Server & server ) { *this = server; }

Server	&Server::operator = ( const Server & server )
{
	if (this != &server)
		;
	return *this;
}

// Methode

void	Server::startup( void )
{
	const int	opt = 1;

	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == -1)
		throw FailedSocket();
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		throw FailedSetsockopt();
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int)) == -1)
		throw FailedSetsockopt();
	_address.sin_family = AF_INET;
	_address.sin_port = htons(80);
	_address.sin_addr.s_addr = INADDR_ANY;
	if (bind(_socket, (const struct sockaddr *)&_address, sizeof(_address)) == -1)
		throw FailedBind();
	if (listen(_socket, 10) == -1)
		throw FailedListen();
}

void	Server::shutdown( void ) const
{
	close(_socket);
}

// ~etter

int	Server::getSocket() const { return _socket; }
struct sockaddr_in	Server::getAddress() const { return _address; }

std::string	Server::getHost() const { return _host; }
int	Server::getPort() const { return _port; }
bool	Server::getDefault() const { return _default; }

std::string	Server::getNames( int idx ) const
{
	if (idx < 0 || idx >= static_cast<int>(_names.size()))
		throw std::out_of_range("Invalid index: Server::getNames()");

	std::list<std::string>::const_iterator	it = _names.begin();
	std::advance(it, idx);
	return *it;
}

std::string	Server::getErrorPages( int idx ) const
{
	std::map<int, std::string>::const_iterator it = _errorPages.find(idx);
	if (it == _errorPages.end())
		throw std::out_of_range("Invalid index: Server::getErrorPages()");
	return it->second;
}

size_t	Server::getMaxSize() const { return _maxSize; }
std::string	Server::getRoot() const { return _root; }

Location	Server::getLocation( int idx ) const
{
	if (idx < 0 || idx >= static_cast<int>(_locations.size()))
		throw std::out_of_range("Invalid index: Server::getLocation()");

	std::list<Location>::const_iterator	it = _locations.begin();
	std::advance(it, idx);
	return *it;
}

void	Server::setPort( int port ) { _port = port; }
