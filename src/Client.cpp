// ************************************************************************** //
//                                                                            //
//                Client.cpp                                                  //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Client.hpp"

Client::Client() : _socket(-1), _server(NULL), _smanager(NULL), _color(Print::getColor(-1)), _wbuf(""), _request(NULL) {}
Client::~Client() { Print::debug(_color, getSocket(), "Logged out."); _backout(); }

Client::Client( int server_socket, Server * server, SessionManager * smanager ) : _socket(-1), _server(server), _smanager(smanager), _wbuf("")
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
	}
	return *this;
}

// Methods

void	Client::read( const std::string & buf )
{
	if (buf.empty())
		return ;
	int status = _request->create(buf);
	if (status != 0)
		return ;
	if (_request->getMethod().find("-") == std::string::npos)
	{
		Print::debug(_color, getSocket(), "New request.");
		Print::debug(_color, getSocket(), "Request :");
		Print::enval(_color, "     | Method", RESET, _request->getMethod());
		Print::enval(_color, "     | Path", RESET, _request->getPath());
	}

	Response response(_request);
	response.build();
	_wbuf = response.string();

	std::ostringstream oss;
	Print::debug(_color, getSocket(), "Response :");
	oss << response.getStatus().first << " " << response.getStatus().second;
	Print::enval(_color, "     | Status", RESET, "[" + std::string(APPLE_GREEN) + oss.str() + std::string(RESET) + "]");
	_request->reset();
}

void	Client::write( void )
{
	if (_wbuf.empty())
		return ;

	ssize_t original = _wbuf.size();
	ssize_t total = 0;
	while (!_wbuf.empty())
	{
		ssize_t n = send(_socket, _wbuf.data(), _wbuf.size(), 0);
		if (n > 0)
			_wbuf.erase(0, n);
		total += n;
	}
	Print::enval(_color, "     | Sent", RESET, "[" + std::string(APPLE_GREEN) + rounded(total) + "/" + rounded(original) + std::string(RESET) + "]");
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
