// ************************************************************************** //
//                                                                            //
//                Client.cpp                                                  //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Client.hpp"
# include "Exceptions.hpp"

Client::Client() : _socket(-1) {}
Client::~Client() { _backout(); }

Client::Client( int server_socket ) { _unit(server_socket); }

// Private Methods

void	Client::_unit( int server_socket )
{
	_socket = accept(server_socket, NULL, NULL);
	if (_socket == -1)
		throw FailedAccept();
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

int	Client::getSocket() const { return _socket; }
