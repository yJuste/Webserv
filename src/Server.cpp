// ************************************************************************** //
//                                                                            //
//                Server.cpp                                                  //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

#include "Exceptions.hpp"
#include "Server.hpp"

Server::Server() : _socket(-1), _host("0.0.0.0"), _root(""), _default(false), _maxSize(1048576)
{
	_index.push_back("index.html");

	_overwritten["index"] = false;
	_overwritten["error_page"] = false;
	_overwritten["client_max_body_size"] = false;
}
Server::~Server() { shutdown(); }

Server::Server( const Server & s ) { *this = s; }

Server	&Server::operator = ( const Server & s )
{
	if (this != &s)
	{
		_socket = -1;
		_address.sin_family = AF_INET;
		_address.sin_port = 0;
		_address.sin_addr.s_addr = INADDR_ANY;
		_host = s.getHost();
		_port = s.getPort();
		_root = s.getRoot();
		_default = s.getDefault();
		_index = s.getIndex();
		_names = s.getNames();
		_errorPages = s.getErrorPages();
		_maxSize = s.getMaxSize();
		_locations = s.getLocations();
		_overwritten = s.getOverwritten();
		_warnings = s.getWarnings();
	}
	return *this;
}

// Methods

void	Server::startup( void )
{
	if (_socket != -1)
		return ;

	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == -1)
		throw FailedSocket();

	const int	opt = 1;

	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		throw FailedSetsockopt();
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int)) == -1)
		throw FailedSetsockopt();

	if (fcntl(_socket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
		throw FailedFcntl();

	struct addrinfo		def;
	struct addrinfo		*info;

	std::memset(&def, 0, sizeof(def));
	def.ai_family = AF_INET;
	def.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(getHost().c_str(), NULL, &def, &info))
		throw FailedGetaddrinfo();

	struct sockaddr_in *add = (struct sockaddr_in *)info->ai_addr;

	_address.sin_family = AF_INET;
	_address.sin_port = htons(getFirstPort());
	_address.sin_addr = add->sin_addr;
	freeaddrinfo(info);

	if (bind(_socket, (const struct sockaddr *)&_address, sizeof(_address)) == -1)
		throw FailedBind();
	if (listen(_socket, 10) == -1)
		throw FailedListen();
}

void	Server::shutdown( void )
{
	if (_socket != -1)
	{
		close(_socket);
		_socket = -1;
	}
}

void	Server::myConfig( void ) const
{

	std::cout << std::endl << BOLD BLUE << "╔════════════════════════════════════════════════════════════╗" << std::endl;
	std::cout << "║                    SERVER CONFIGURATION                    ║" << std::endl;
	std::cout << "╚════════════════════════════════════════════════════════════╝" << "\033[0m" << std::endl << std::endl;
	std::cout << BROWN << " Host              : " << BEIGE << getHost() << RESET << std::endl;
	std::cout << BROWN << " Port(s)           : " << BEIGE;
	for ( std::vector<int>::const_iterator it = getPort().begin(); it != getPort().end(); ++it )
		std::cout << *it << " ";
	std::cout << RESET << std::endl;
	std::cout << BROWN << " Root              : " << BEIGE << getRoot() << RESET << std::endl;
	std::cout << BROWN << " Index             : " << BEIGE;
	for ( std::vector<std::string>::const_iterator it = getIndex().begin(); it != getIndex().end(); ++it )
		std::cout << *it << " ";
	std::cout << RESET << std::endl;
	std::cout << BROWN << " Default Server    : " << BEIGE << ( getDefault() ? "\033[32mYes\033[0m" : "\033[31mNo\033[0m" ) << RESET << std::endl;
	std::cout << BROWN << " Max Body Size     : " << BEIGE << _rounded(getMaxSize()) << " bytes" << RESET << std::endl;
	std::cout << std::endl << BROWN << " Server Name(s) :" << RESET << std::endl;
	for ( std::vector<std::string>::const_iterator it = getNames().begin(); it != getNames().end(); ++it )
		std::cout << BEIGE << "   - " << *it << RESET << std::endl;
	std::cout << std::endl << BROWN << " Error Page(s) :" << RESET;
	if (getErrorPages().empty())
		std::cout << YELLOW << " No specified." << RESET << std::endl;
	else
	{
		std::cout << std::endl;
		for ( std::map<int, std::string>::const_iterator it = getErrorPages().begin(); it != getErrorPages().end(); ++it )
			std::cout << BEIGE << "   [" << RESET << BLUE << std::setw(3) << it->first << RESET << BEIGE << "] => " << RESET << YELLOW << it->second << RESET << std::endl;
	}
	std::cout << std::endl << BOLD BLUE << "═══ LOCATIONS ════════════════════════════════════════════════" << RESET << std::endl;
	for ( size_t i = 0; i < getLocations().size(); ++i )
	{
		const Location	&loc = _locations[i];
		std::cout << std::endl;
		std::cout << BLUE << " Location Path         " << YELLOW << loc.getPath() << RESET << std::endl << std::endl;
		std::cout << BROWN << "    - Root             : " << BEIGE << loc.getRoot() << RESET << std::endl;
		std::cout << BROWN << "    - Method(s)        : " << RESET;
		std::vector<std::string> methods = loc.getMethods();
		for ( std::vector<std::string>::const_iterator mit = methods.begin(); mit != methods.end(); ++mit )
			std::cout << BEIGE << *mit << " " << RESET;
		std::cout << std::endl;
		std::map<int, std::string>::const_iterator rit = loc.getReturn().begin();
		std::cout << BROWN << "    - Redirection      : " << RESET;
		if (rit != loc.getReturn().end())
			std::cout << BLUE << rit->first << BEIGE << " " << rit->second;
		else
			std::cout << "\033[31mDisabled\033[0m";
		std::cout << RESET << std::endl;
		std::cout << BROWN << "    - Default File(s)  : " << BEIGE;
		std::vector<std::string>	index = loc.getIndex();
		for ( std::vector<std::string>::const_iterator it = index.begin(); it != index.end(); ++it )
			std::cout << *it << " ";
		std::cout << RESET << std::endl;
		std::cout << BROWN << "    - Autoindex        : " << BEIGE << (loc.getAutoindex() ? "\033[32mEnabled\033[0m" : "\033[31mDisabled\033[0m") << RESET << std::endl;
		std::cout << BROWN << "    - Upload Folder    : " << BEIGE << loc.getUpload() << RESET << std::endl;
		std::map<std::string, std::string>	cgi = loc.getCgi();
		if (!cgi.empty())
		{
			std::cout << BROWN << "    - Available CGI    :" << RESET << std::endl;
			for ( std::map<std::string, std::string>::const_iterator cit = cgi.begin(); cit != cgi.end(); ++cit )
				if (!cit->second.empty())
					std::cout << BEIGE << "        - " << BLUE << cit->first << BEIGE << " => " << YELLOW << cit->second << RESET << std::endl;
		}
	}
	std::cout << std::endl << BOLD BLUE << "═══ WARNINGS ═════════════════════════════════════════════════" << RESET << std::endl << std::endl;
	std::vector<std::string>	warnings = getWarnings();
	for ( std::vector<std::string>::const_iterator wit = warnings.begin(); wit != warnings.end(); ++wit )
		std::cerr << " " << *wit << std::endl;
	if (warnings.empty())
		std::cout << GREEN << " ➤ This server configuration is now ready." << RESET << std::endl;
	std::cout << std::endl << BOLD BLUE << "══════════════════════════════════════════════════════════════" << RESET << std::endl << std::endl;
}

// Getters

int Server::getSocket() const { return _socket; }
const struct sockaddr_in & Server::getAddress() const { return _address; }
const std::string & Server::getHost() const { return _host; }
const std::vector<int> & Server::getPort() const { return _port; }
int Server::getFirstPort() const { if (!_port.size()) return -1; return _port[0]; }
const std::string & Server::getRoot() const { return _root; }
bool Server::getDefault() const { return _default; }
const std::vector<std::string> & Server::getIndex() const { return _index; }
const std::vector<std::string> & Server::getNames() const { return _names; }
const std::map<int, std::string> & Server::getErrorPages() const { return _errorPages; }
size_t Server::getMaxSize() const { return _maxSize; }
const std::vector<Location> & Server::getLocations() const { return _locations; }
const std::map<std::string, bool> & Server::getOverwritten() const { return _overwritten; }
bool Server::getOverwrittenX( const std::string & parameter ) const { std::map<std::string, bool>::const_iterator it = _overwritten.find(parameter); return it != _overwritten.end() && it->second; }
const std::vector<std::string> & Server::getWarnings() const { return _warnings; }

// Setters

void Server::setHost( const std::string & host ) { _host = host; }
void Server::setPort( const std::vector<int> & port ) { _port = port; }
void Server::addPort( int port ) { for (size_t i = 0; i < _port.size(); ++i) if (_port[i] == port) return ; _port.push_back(port); }
void Server::setRoot( const std::string & root ) { _root = root; }
void Server::setDefault( bool def ) { _default = def; }
void Server::setIndex( const std::vector<std::string> & index ) { _index = index; }
void Server::setNames( const std::vector<std::string> & names ) { _names = names; }
void Server::addErrorPage( int code, const std::string & path ) { _errorPages[code] = path; }
void Server::setMaxSize( int size ) { _maxSize = size; }
void Server::addLocation( const Location & location ) { _locations.push_back(location); }
void Server::setOverwritten( const std::string & parameter ) { _overwritten[parameter] = true; }
void Server::addWarning( const std::string & warning ) { _warnings.push_back(warning); }

// Private Methods

// return rounded number with its nearest units.
std::string	Server::_rounded( size_t bytes ) const
{
	const char * units[] = {"bytes", "KB", "MB", "GB", "TB"};
	double size = static_cast<double>(bytes);
	int unit = 0;

	while ( size >= 1024.0 && unit < 4 )
	{
		size /= 1024.0;
		++unit;
	}

	std::stringstream	ss;

	ss.precision(2);
	ss.setf(std::ios::fixed);
	ss << size << " " << units[unit];
	return ss.str();
}
