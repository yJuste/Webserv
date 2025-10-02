// ************************************************************************** //
//                                                                            //
//                Client.cpp                                                  //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Client.hpp"

Client::Client() : _socket(-1), _server(NULL), _smanager(NULL), _color(Print::getColor(-1)), _wbuf(""), _request(NULL), _sv(-1), _original(0), _total(0) {}
Client::~Client() { Print::debug(_color, getSocket(), "Logged out."); _backout(); }

Client::Client( int server_socket, Server * server, SessionManager * smanager ) : _socket(-1), _server(server), _smanager(smanager), _wbuf(""), _sv(-1), _original(0), _total(0)
{
	_unit(server_socket);
	_color = Print::getColor(_socket);
	_request = new Request(this);
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
		_sv = c._sv;
	}
	return *this;
}

// Methods

int	Client::retrieve( const std::string & buf )
{
	if (buf.empty())
		return 0;
	int status = _request->create(buf);
	if (status != 0)
		return 0;
	if (_request->getMethod().find("-") == std::string::npos)
	{
		Print::debug(_color, getSocket(), "New request.");
		Print::debug(_color, getSocket(), "Request :");
		Print::enval(_color, "     | Method", RESET, _request->getMethod());
		Print::enval(_color, "     | Path", RESET, _request->getPath());
	}
	_original = 0;
	_total = 0;

	Response response(_request);
	response.build();
	_wbuf = response.string();
	if (_sv >= 0)
		return 1;

	std::ostringstream oss;
	Print::debug(_color, getSocket(), "Response :");
	oss << response.getStatus().first << " " << response.getStatus().second;
	Print::enval(_color, "     | Status", RESET, "[" + std::string(APPLE_GREEN) + oss.str() + std::string(RESET) + "]");
	_request->reset();
	return 0;
}

int	Client::receive( int fd, char * buffer, size_t size )
{
	return recv(fd, buffer, size, 0);
}

ssize_t	Client::write( void )
{
	if (_wbuf.empty())
		return 0;

	ssize_t n = send(_socket, _wbuf.data(), _wbuf.size(), 0);
	if (n > 0)
		_wbuf.erase(0, n);
	_total += n;
	return n;
}

void	Client::sent( void )
{
	Print::enval(_color, "     | Sent", RESET, "[" + std::string(APPLE_GREEN) + rounded(_total) + "/" + rounded(_original) + std::string(RESET) + "]");
	_wbuf.clear();
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
int Client::getSv() const { return _sv; }

// Setters

void Client::setSv( int sv ) { _sv = sv; }
void Client::setOriginal( ssize_t original ) { _original = original; }
