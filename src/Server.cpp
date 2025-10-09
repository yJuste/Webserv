// ************************************************************************** //
//                                                                            //
//                Server.cpp                                                  //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Server.hpp"

Server::Server() : _socket(-1), _host(""), _root(""), _default(true), _maxSize(1048576)
{
	_port.push_back(80);
	_index.push_back("index.html");
	_overwritten["host"] = 0;
	_overwritten["index"] = 0;
	_overwritten["error_page"] = 0;
	_overwritten["client_max_body_size"] = 0;
}
Server::~Server() { shutdown(); }

Server::Server( const Server & s ) { *this = s; }

Server	& Server::operator = ( const Server & s )
{
	if (this != &s)
	{
		_socket = -1;
		_address.sin_family = AF_INET;
		_address.sin_port = 0;
		_address.sin_addr.s_addr = INADDR_ANY;
		_host = s._host;
		_port = s._port;
		_root = s._root;
		_default = s._default;
		_index = s._index;
		_names = s._names;
		_errorPages = s._errorPages;
		_maxSize = s._maxSize;
		_locations = s._locations;
		_warnings = s._warnings;
		_overwritten = s._overwritten;
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

	const int opt = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		throw FailedSetsockopt();
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int)) == -1)
		throw FailedSetsockopt();
	if (fcntl(_socket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
		throw FailedFcntl();

	struct addrinfo def;
	struct addrinfo * info;
	std::memset(&def, 0, sizeof(def));
	def.ai_family = AF_INET;
	def.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(_host.c_str(), NULL, &def, &info))
		throw FailedGetaddrinfo();

	struct sockaddr_in * add = (struct sockaddr_in *)info->ai_addr;
	std::memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_port = htons(getPort());
	_address.sin_addr = add->sin_addr;
	freeaddrinfo(info);
	if (bind(_socket, (const struct sockaddr *)&_address, sizeof(_address)) == -1)
		throw FailedBind();
	if (listen(_socket, SOMAXCONN) == -1)
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
	Print::header("SERVER CONFIGURATION", BLUE);
	Print::enval(BROWN, "Host", BEIGE, getHost() == "127.0.0.1" ? "localhost" : getHost() );
	Print::enval(BROWN, "Port", BEIGE, getPort());
	const std::string & s_root = getRoot();
	Print::enval(BROWN, "Root", BEIGE, s_root);
	Print::entry(BROWN, "Index");
	const std::vector<std::string> & s_index = getIndex();
	for (std::vector<std::string>::const_iterator it = s_index.begin(); it != s_index.end(); ++it)
	{
		if (it + 1 != s_index.end())
			Print::value(BEIGE, to_clean(s_root, *it) + ", ");
		else
			Print::value(BEIGE, to_clean(s_root, *it));
	}
	Print::endl();
	Print::enval(BROWN, "Default Server", BEIGE, getDefault() ? APPLE_GREEN "Yes" RESET : RED "No" RESET);
	Print::enval(BROWN, "Max Body Server", BEIGE, rounded(getMaxSize()));
	Print::entry(BROWN, "Server Names(s)");
	const std::vector<std::string> & s_names = getNames();
	for (std::vector<std::string>::const_iterator it = s_names.begin(); it != s_names.end(); ++it)
	{
		if (it + 1 != s_names.end())
			Print::value(BEIGE, *it + ", ");
		else
			Print::value(BEIGE, *it);
	}
	Print::endl();
	Print::entry(BROWN, "Error Page(s)");
	const std::map<int, std::string> & s_errorPages = getErrorPages();
	if (s_errorPages.empty())
		{ Print::value(YELLOW, "No specified."); Print::endl(); }
	else
	{
		Print::endl();
		for (std::map<int, std::string>::const_iterator it = s_errorPages.begin(); it != s_errorPages.end(); ++it)
		{
			Print::value(BROWN, "   [");
			std::cout << BLUE << std::right << std::setw(3) << it->first;
			Print::value(BROWN, "] => "); Print::value(YELLOW, it->second); Print::endl();
		}
	}
	Print::subPart("LOCATION", BLUE);
	for (size_t i = 0; i < getLocations().size(); ++i)
	{
		const Location & loc = _locations[i];
		Print::endl();
		Print::enval(BLUE, "Location Path", YELLOW, "   " + loc.getPath()); Print::endl();
		std::map<int, std::string>::const_iterator rit = loc.getReturn().begin();
		if (rit != loc.getReturn().end())
		{
			Print::entry(BROWN, "\tRedirection");
			Print::value(APPLE_GREEN, rit->first);
			Print::value(BEIGE, " " + rit->second);
			Print::endl();
		}
		else
		{
			const std::string & l_root = loc.getRoot();
			Print::enval(BROWN, "\tRoot", BEIGE, to_clean(s_root, l_root));
			Print::entry(BROWN, "\tDefault File(s)");
			const std::vector<std::string> & l_index = loc.getIndex();
			for (std::vector<std::string>::const_iterator it = l_index.begin(); it != l_index.end(); ++it)
			{
				if (it + 1 != l_index.end())
					Print::value(BEIGE, to_clean(l_root, *it) + ", ");
				else
					Print::value(BEIGE, to_clean(l_root, *it));
			}
			Print::endl();
			const std::string & l_upload = loc.getUpload();
			if (l_upload != l_root)
				Print::enval(BROWN, "\tUpload Folder", BEIGE, to_clean(l_root, l_upload));
			size_t l_maxSize = loc.getMaxSize();
			if (l_maxSize != 1048576)
				Print::enval(BROWN, "\tMax Body Server", BEIGE, rounded(l_maxSize));
			Print::entry(BROWN, "\tMethod(s)");
			const std::vector<std::string> & l_methods = loc.getMethods();
			if (l_methods.size() != 3)
			{
				for (std::vector<std::string>::const_iterator mit = l_methods.begin(); mit != l_methods.end(); ++mit)
				{
					if (mit + 1 != l_methods.end())
						Print::value(BLUE, *mit + " ");
					else
						Print::value(BLUE, *mit);
				}
			}
			else
				Print::value(BLUE, "all");
			Print::endl();
			const std::map<std::string, std::string> & l_cgi = loc.getCgi();
			if (!l_cgi.empty())
			{
				Print::entry(BROWN, "\tAvailable CGI");
				Print::endl();
				for (std::map<std::string, std::string>::const_iterator cit = l_cgi.begin(); cit != l_cgi.end(); ++cit)
					if (!cit->second.empty())
						std::cout << "\t\t" << BLUE << cit->first << BEIGE << " => " << YELLOW << cit->second << RESET << std::endl;
			}
		}
	}
	Print::subPart("WARNING", BLUE); Print::endl();
	const std::vector<std::string> & s_warnings = getWarnings();
	if (s_warnings.empty())
	{
		Print::value(APPLE_GREEN, " ➤ This server configuration is now ready.");
		Print::endl();
	}
	else
		for (std::vector<std::string>::const_iterator wit = s_warnings.begin(); wit != s_warnings.end(); ++wit)
			std::cerr << " " << *wit << std::endl;
	Print::subPart("", BLUE);
}

// Getters

int Server::getSocket() const { return _socket; }
const struct sockaddr_in & Server::getAddress() const { return _address; }
const std::string & Server::getHost() const { return _host; }
const std::vector<int> & Server::getEveryPort() const { return _port; }
int Server::getPort() const { if (!_port.size()) return -1; return _port[0]; }
std::string Server::getHostPort() const { std::ostringstream oss; oss << _host << ":" << getPort(); return oss.str(); }
const std::string & Server::getRoot() const { return _root; }
bool Server::getDefault() const { return _default; }
const std::vector<std::string> & Server::getIndex() const { return _index; }
const std::vector<std::string> & Server::getNames() const { return _names; }
const std::map<int, std::string> & Server::getErrorPages() const { return _errorPages; }
size_t Server::getMaxSize() const { return _maxSize; }
const std::vector<Location> & Server::getLocations() const { return _locations; }
const Location * Server::getXLocation( const std::string & path ) const
{
	for (std::vector<Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it)
		if (it->getPath() == path) return &(*it);
	return NULL;
}
const std::map<std::string, int> & Server::getOverwritten() const { return _overwritten; }
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
void Server::setMaxSize( size_t size ) { _maxSize = size; }
void Server::addLocation( const Location & location ) { _locations.push_back(location); }
void Server::setOverwritten( const std::string & parameter ) { _overwritten[parameter] += 1; }
void Server::addWarning( const std::string & warning ) { _warnings.push_back(warning); }
