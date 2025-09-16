// ************************************************************************** //
//                                                                            //
//                Response.cpp                                                //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Layang                                        //
//                                                                            //
// ************************************************************************** //

# include "Response.hpp"
# include "Request.hpp"
# include "Client.hpp"

Response::Response() : _req(NULL), _server(NULL), _client(NULL), _status(200, "OK"), _body("") {}
Response::~Response() {}

Response::Response( Request * req ) : _req(req), _status(200, "OK"), _body("")
{
	_client = _req->getClient();
	_server = _client->getServer();
}

// Methods

std::string	Response::build( void )
{
	const Location * loc = _findLocation();
	if (!loc)
	{
		_setStatus(404, "Not Found");
		const std::map<int, std::string> & pages = _server->getErrorPages();
		if (pages.find(404) != pages.end())
			_setBody(readFile(pages.at(404)));
		else
			_setBody("No matching location");
		return _reconstitution();
	}
	_setBody(readFile(loc->getIndex()[0]));
	return _reconstitution();
}

std::string	Response::_reconstitution( void ) const
{
	std::stringstream response;
	const Location * loc = _findLocation();

	response << "HTTP/1.1 " << _status.first << " " << _status.second << "\r\n";

	if (loc)
	{
		std::string type = getContentType(loc->getIndex()[0]);
		response << "Content-Type: " << type << "\r\n";		
	}
	else
	{
		std::string type = getContentType(_server->getIndex()[0]);
		response << "Content-Type: " << type << "\r\n";		
	}

	std::stringstream ss;

	ss << _body.size();
	response << "Content-Length: " << ss.str() << "\r\n";
	response << "Connection: keep-alive" << "\r\n";
	response << "\r\n";
	response << _body;
	return response.str();
}

// Private Methods

const Location *	Response::_findLocation( void ) const
{
	const std::vector<Location> & locations = _server->getLocations();
	const Location * best = NULL;

	for (size_t i = 0; i < locations.size(); ++i)
		if (_req->getPath().find(locations[i].getPath()) == 0)
			if (!best || locations[i].getPath().size() > best->getPath().size())
				best = &locations[i];
	return best;
}

std::string	Response::getContentType( const std::string & path ) const
{
	std::cout << APPLE_GREEN << path << RESET << std::endl;
	if (path.length() >= 5 && path.substr(path.length() - 5) == ".html")
		return "text/html; charset=UTF-8";
	if (path.length() >= 4 && path.substr(path.length() - 4) == ".htm")
		return "text/html; charset=UTF-8";
	if (path.length() >= 4 && path.substr(path.length() - 4) == ".css")
		return "text/css; charset=UTF-8";
	if (path.length() >= 3 && path.substr(path.length() - 3) == ".js")
		return "application/javascript; charset=UTF-8";
	if (path.length() >= 4 && path.substr(path.length() - 4) == ".jpg")
		return "image/jpeg";
	if (path.length() >= 5 && path.substr(path.length() - 5) == ".jpeg")
		return "image/jpeg";
	if (path.length() >= 4 && path.substr(path.length() - 4) == ".png")
		return "image/png";
	if (path.length() >= 4 && path.substr(path.length() - 4) == ".gif")
		return "image/gif";
	return "text/plain; charset=UTF-8";
}

// Setters

void Response::_setStatus( int status, const std::string & statusText ) { _status.first = status; _status.second = statusText; }
void Response::_setBody( const std::string & body ) { _body = body; }
