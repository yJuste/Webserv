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

Server::Server() : _socket(-1), _host("0.0.0.0"), _port(80), _root(""), _default(false), _maxSize(0) {}
Server::~Server() {}

Server::Server( const Server & server ) { *this = server; }

Server	&Server::operator = ( const Server & s )
{
	if (this != &s)
	{
		_socket = s.getSocket();
		_address = s.getAddress();
		_host = s.getHost();
		_port = s.getPort();
		_root = s.getRoot();
		_default = s.getDefault();
		_names = s.getNames();
		_errorPages = s.getErrorPages();
		_maxSize = s.getMaxSize();
		_locations = s.getLocations();
	}
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

	struct addrinfo		def;
	struct addrinfo		*info;

	std::memset(&def, 0, sizeof(def));
	def.ai_family = AF_INET;
	def.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(getHost().c_str(), NULL, &def, &info))
		throw FailedGetaddrinfo();

	struct sockaddr_in *add = (struct sockaddr_in *)info->ai_addr;

	_address.sin_family = AF_INET;
	_address.sin_port = htons(getPort());
	_address.sin_addr = add->sin_addr;
	freeaddrinfo(info);

	if (bind(_socket, (const struct sockaddr *)&_address, sizeof(_address)) == -1)
		throw FailedBind();
	if (listen(_socket, 10) == -1)
		throw FailedListen();
}

void	Server::shutdown( void ) const
{
	close(_socket);
}

// Getter

int					Server::getSocket() const { return _socket; }
const struct sockaddr_in		&Server::getAddress() const { return _address; }

const std::string			&Server::getHost() const { return _host; }
int					Server::getPort() const { return _port; }
const std::string			&Server::getRoot() const { return _root; }
bool					Server::getDefault() const { return _default; }
const std::vector<std::string>		&Server::getNames() const { return _names; }
const std::map<int, std::string>	&Server::getErrorPages() const { return _errorPages; }
size_t					Server::getMaxSize() const { return _maxSize; }
const std::vector<Location>		&Server::getLocations() const { return _locations; }

const std::string			&Server::getNameX( int idx ) const
{
	if (idx < 0 || idx >= static_cast<int>(_names.size()))
		throw FailedGetNameX();

	std::vector<std::string>::const_iterator	it = _names.begin();
	std::advance(it, idx);
	return *it;
}

const std::string			&Server::getErrorPageX( int idx ) const
{
	std::map<int, std::string>::const_iterator it = _errorPages.find(idx);
	if (it == _errorPages.end())
		throw FailedGetErrorPageX();
	return it->second;
}

const Location				&Server::getLocationX( int idx ) const
{
	if (idx < 0 || idx >= static_cast<int>(_locations.size()))
		throw FailedGetLocationX();

	std::vector<Location>::const_iterator	it = _locations.begin();
	std::advance(it, idx);
	return *it;
}

// Setter

void	Server::setHost( const std::string & host ) { _host = host; }
void	Server::setPort( int port ) { _port = port; }
void	Server::setRoot( const std::string & root ) { _root = root; }
void	Server::setDefault( bool def ) { _default = def; }
void	Server::setNames( const std::vector<std::string> & names ) { _names = names; }
void	Server::setErrorPages( const std::map<int, std::string> & errorPages ) { _errorPages = errorPages; }
void	Server::setMaxSize( int size ) { _maxSize = size; }
