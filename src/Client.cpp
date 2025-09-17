// ************************************************************************** //
//                                                                            //
//                Client.cpp                                                  //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Client.hpp"

Client::Client() : _socket(-1), _server(NULL), _color(Print::getColor(-1)), _wbuf(""), _request(NULL) {}
Client::~Client() { Print::debug(_color, getSocket(), "Logged out."); _backout(); }

Client::Client( int server_socket, Server * server ) : _socket(-1), _server(server), _wbuf("")
{
	_unit(server_socket);
	_color = Print::getColor(_socket);
	_request = new Request(this);
	Print::debug(_color, getSocket(), "Logged in.");
}

// Methods

void	Client::read( const std::string & buf )
{
	Print::debug(_color, getSocket(), "New request.");
	_wbuf.clear();
	if (_request->create(buf) == 2)
		return ;
	Print::debug(_color, getSocket(), "Request completed :");
	Print::enval(_color, "    | Method", RESET, _request->getMethod());
	Print::enval(_color, "    | Path", RESET, _request->getPath());
}

void	Client::write( void )
{
	Response	response(_request);

	response.build();
	_wbuf = response.string();
	Print::debug(_color, getSocket(), "Reconstitution done.");
	int n = send(_socket, _wbuf.data(), _wbuf.size(), 0);
	if (n <= 0)
		Print::debug(RED, "Client", "The client encountered errors during writing.");
	Print::debug(_color, getSocket(), "Sent.");
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
const char * Client::getColor() const { return _color; }
