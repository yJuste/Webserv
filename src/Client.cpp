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

Client::Client(int server_socket, Server* server) 
    : _socket(-1), _server(server)
{
    _unit(server_socket);
}

// Private Methods

/* void	Client::_unit( int server_socket )
{
	if (_socket != -1)
		return ;
	_socket = accept(server_socket, NULL, NULL);
	if (_socket == -1)
		throw FailedAccept();
	if (fcntl(_socket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
		throw FailedFcntl();
} */

void Client::_unit(int server_socket)
{
    if (_socket != -1)
        return;

    // Try to accept a new connection, do not throw an exception if it fails
    int new_socket = accept(server_socket, NULL, NULL);
    if (new_socket < 0)
    {
        _socket = -1; // Mark as not connected
        return;       // If accept fails, just return
    }

    _socket = new_socket;

    // Set the socket to non-blocking mode with close-on-exec
    if (fcntl(_socket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
    {
        close(_socket);
        _socket = -1;
        throw FailedFcntl();
    }
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


// Data treating
bool Client::hasDataToWrite() const { return !_writebBuffer.empty(); }

void Client::readFromClient(const char * buffer, int n)
{
    /*     
    char buffer[READ_SIZE];
    int n = recv(_socket, buffer, sizeof(buffer), 0);
    if (n == 0)
        return false;
    else if (n == -1)
        return true; 
    */
    //std::cout << "Request received: " << std::endl; ///
    //std::cout << buffer << std::endl;   ///
    _readBuffer.append(buffer, n);
    _request.parseRequest(_readBuffer);

    if (_request.isComplete()) {
        // print important info
		if (!_request.hasPrinted())
			_request.setPrinted(true);
		std::cout << "      ----****  new request ****----: " << std::endl;	
        std::cout << "Request complete: "
                  << _request.getMethod() << " "
                  << _request.getPath() << std::endl;
        // print headers
        const std::map<std::string, std::string>& headers = _request.getHeaders();
        for (std::map<std::string, std::string>::const_iterator it = headers.begin();
             it != headers.end(); ++it)
        {
            std::cout << it->first << ": " << it->second << std::endl;
        }
        
        HttpResponse response;
        //response.setLocations(_server->getLocations());
        response.buildResponse(_request, _server);    //
        _writebBuffer = response.toString(_request);
        _readBuffer.clear();
        _request.reset();
    }
}

/* void Client::writeToClient()
{
    if (_writebBuffer.empty()) return;

    int n = send(_socket, _writebBuffer.data(), _writebBuffer.size(), 0);
    // if (n == 0) return false;

    // _writebBuffer.erase(0, n);
    // return true;

    if (n > 0)
        _writebBuffer.erase(0, n);

} */

void Client::writeToClient()
{
    while (!_writebBuffer.empty())
    {
        int n = send(_socket, _writebBuffer.data(), _writebBuffer.size(), 0);
        if (n > 0)
            _writebBuffer.erase(0, n);
        else
            break;
    }
}

