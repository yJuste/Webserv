// ************************************************************************** //
//                                                                            //
//                Client.cpp                                                  //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Client.hpp"

Client::Client() : _socket(-1), _server(NULL), _smanager(NULL), _color(Print::getColor(-1)), _wbuf(""), _request(NULL), _original(0), _total(0), _svRead(-1), _svWrite(-1), _cgiSent(0), _cgiSending(false), _cgiPid(-1), _cgiStart(-1) {}
Client::~Client() { Print::debug(_color, getSocket(), "Logged out."); _backout(); }

Client::Client( int server_socket, SessionManager * smanager ) : _socket(-1), _server(NULL), _smanager(smanager), _wbuf(""), _original(0), _total(0), _svRead(-1), _svWrite(-1), _cgiSent(0), _cgiSending(false), _cgiPid(-1), _cgiStart(-1)
{
	_request = new Request(this);
	_unit(server_socket);
	_color = Print::getColor(_socket);
	Print::debug(_color, getSocket(), "Logged in.");
}

Client::Client( const Client & c ) { *this = c; }

Client	& Client::operator = ( const Client & c )
{
	if (this != &c)
	{
		_socket = -1;
		_server = c._server;
		_smanager = c._smanager;
		_color = c._color;
		_wbuf = c._wbuf;
		_request = c._request;
		_original = c._original;
		_total = c._total;
		_svRead = c._svRead;
		_svWrite = c._svWrite;
		_cgiBody = c._cgiBody;
		_cgiSent = c._cgiSent;
		_cgiSending = c._cgiSending;
	}
	return *this;
}

// Methods

int	Client::retrieve( const std::string & buf )
{
	if (buf.empty())
		return 0;
	if (_request->create(buf) != 0)
		return 0;
	if (_request->getMethod().find("-") == std::string::npos)
	{
		Print::debug(_color, getSocket(), "New request." + std::string(_color) + "                                    .......");
		Print::debug(_color, getSocket(), "Request :" + std::string(_color) + "                                             .");
		Print::enval(_color, "     | Method", RESET, _request->getMethod());
		Print::enval(_color, "     | Path", RESET, _request->getPath());
	}
	_original = 0;
	_total = 0;
	return 1;
}

int	Client::response( void )
{
	Response response(_request);
	response.build();
	_wbuf = response.string();
	if (_svWrite >= 0 || _svRead >= 0)
		return 1;

	std::ostringstream oss;
	Print::debug(_color, getSocket(), "Response :");
	oss << response.getStatus().first << " " << response.getStatus().second;
	Print::enval(_color, "     | Status", RESET, "[" + std::string(APPLE_GREEN) + oss.str() + std::string(RESET) + "]");
	_request->reset();
	return 0;
}

ssize_t	Client::receive( int fd, char * buffer, size_t size )
{
	return recv(fd, buffer, size, 0);
}

ssize_t	Client::dispatch( int fd, const char * buffer, size_t size )
{
	return send(fd, buffer, size, 0);
}

ssize_t	Client::writing( void )
{
	ssize_t	n;
	if (_svWrite >= 0 && isCgiSending())
	{
		n = dispatch(_svWrite, &_cgiBody[_cgiSent], _cgiBody.size() - _cgiSent);
		if (n > 0)
		{
			_cgiSent += n;
			if (_cgiSent == _cgiBody.size())
			{
				_cgiSending = false;
				close(_svWrite);
				_svWrite = -1;
			}
		}
		return n;
	}
	if (_wbuf.empty())
		return 0;
	n = dispatch(_socket, _wbuf.data(), _wbuf.size());
	if (n > 0)
		_wbuf.erase(0, n);
	_total += n;
	return n;
}

void	Client::sent( void )
{
	Print::enval(_color, "     | Page", RESET, "[" + std::string(APPLE_GREEN) + rounded(_total) + "/" + rounded(_original) + std::string(RESET) + "]");
	Print::enval(_color, "     | Sent", _color, "                                         .");
	_wbuf.clear();
}

Server *	Client::select_server( const std::vector<Server *> & servers )
{
	std::string host = _request->getHeader("Host");
	int port = -1;
	size_t pos = host.find(':');
	if (pos != std::string::npos)
	{
		std::string portStr = host.substr(pos + 1);
		host = host.substr(0, pos);
		if (!portStr.empty())
		{
			std::istringstream iss(portStr);
			iss >> port;
			if (iss.fail())
				port = -1;
		}
	}
	if (port == -1)
	{
		struct sockaddr_in addr;
		socklen_t len = sizeof(addr);
		if (getsockname(_socket, (struct sockaddr *)&addr, &len) == 0)
			port = ntohs(addr.sin_port);
	}
	std::vector<Server *> candidates;
	for (size_t i = 0; i < servers.size(); ++i)
	{
		const std::vector<int> & ports = servers[i]->getEveryPort();
		for (size_t j = 0; j < ports.size(); ++j)
		{
			if (ports[j] == port)
			{
				candidates.push_back(servers[i]);
				break;
			}
		}
	}
	if (candidates.empty())
		return servers[0];
	for (size_t i = 0; i < candidates.size(); ++i)
	{
		const std::vector<std::string> & names = candidates[i]->getNames();
		for (size_t j = 0; j < names.size(); ++j)
			if (names[j] == host)
				return candidates[i];
	}
	for (size_t i = 0; i < candidates.size(); ++i)
		if (candidates[i]->getDefault())
			return candidates[i];
	return candidates[0];
}

// Private Methods

void	Client::_unit( int server_socket )
{
	if (_socket != -1)
		return ;
	_socket = accept(server_socket, NULL, NULL);
	if (_socket == -1)
		throw FailedAccept();
	if (fcntl(_socket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
		throw FailedFcntl();
}

void	Client::_backout( void )
{
	if (_socket != -1)
	{
		close(_socket);
		_socket = -1;
	}
	delete _request;
}

// Getters

int Client::getSocket() const { return _socket; }
const Server * Client::getServer() const { return _server; }
SessionManager * Client::getSessionManager() const { return _smanager; }
const char * Client::getColor() const { return _color; }
std::string & Client::wbuf() { return _wbuf; }
Request * Client::getRequest() { return _request; }
int Client::getSvRead() const { return _svRead; }
int Client::getSvWrite() const { return _svWrite; }
const std::vector<char> & Client::getCgiBody() const { return _cgiBody; }
bool Client::isCgiSending() const { return _cgiSending && _cgiSent < _cgiBody.size(); }
pid_t Client::getCgiPid() const { return _cgiPid; }
time_t Client::getCgiStart() const { return _cgiStart; }

// Setters

void Client::setServer( Server * server ) { _server = server; }
void Client::setOriginal( ssize_t original ) { _original = original; }
void Client::setSvWrite( int fd ) { _svWrite = fd; }
void Client::setSvRead( int fd ) { _svRead = fd; }
void Client::setCgiBody( const std::vector<char> & body ) { _cgiBody = body; _cgiSent = 0; _cgiSending = true; }
void Client::setCgiPid( pid_t pid ) { _cgiPid = pid; }
void Client::setCgiStart( time_t start ) { _cgiStart = start; }
