// ************************************************************************** //
//                                                                            //
//                Client.cpp                                                  //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Client.hpp"

Client::Client() : _socket(-1), _server(0) {}
Client::~Client() { _backout(); }

Client::Client( int server_socket, Server * server ) : _socket(-1), _server(server) { _unit(server_socket); }

// Methods

void	Client::read( const std::string & buf )
{
	_clean();
	_request.parse(buf);
	if (!_request.isComplete())
		return ;
	Print::debug(APPLE_GREEN, getSocket(), "New request.");
	if (!_request.getPrinted())
		_request.setPrinted(true);
	Print::debug(APPLE_GREEN, getSocket(), "Request completed :");
	Print::enval(APPLE_GREEN, "Method", RESET, _request.getMethod());
	Print::enval(APPLE_GREEN, "Path", RESET, _request.getPath());
	Print::enval(APPLE_GREEN, "Version", RESET, _request.getVersion());
        for (std::map<std::string, std::string>::const_iterator it = _request.getHeaders().begin(); it != _request.getHeaders().end(); ++it)
		Print::enval(APPLE_GREEN, it->first + ":", RESET, it->second);
}

void	Client::write( void )
{
	Response	response;

	response.build(_request, _server);
	_wbuf = response.reconstitution(_request, _server);
	Print::debug(BLUE, "Response", "Reconstitution done.");
	while (!_wbuf.empty())
	{
		int n = send(_socket, _wbuf.data(), _wbuf.size(), 0);
		if (n > 0)
			_wbuf.erase(0, n);
		else
			Print::debug(RED, "Client", "The client encountered errors during writing.");
	}
	Print::debug(BLUE, "Response", "Sent.");
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

void	Client::_clean( void )
{
	_wbuf.clear();
        _request.reset();
}

void	Client::_backout( void )
{
	if (_socket != -1)
	{
		close(_socket);
		_socket = -1;
	}
}

// Getter

int Client::getSocket() const { return _socket; }
