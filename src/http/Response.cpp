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

void	Response::build( void )
{
	bool location = _preparation();
	if (location)
		return _reconstitution();
}

std::string	Response::string( void ) const
{
	std::stringstream response;

	response << "HTTP/1.1 " << _status.first << " " << _status.second << "\r\n";
	if (_headers.find("Content-Type") == _headers.end())
	{
		std::string type = getContentType(_req->getPath());
		response << "Content-Type: " << type << "; charset=UTF-8\r\n";
	}
	std::stringstream ss;
	ss << _body.size();
	response << "Content-Length: " << ss.str() << "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		response << it->first << ": " << it->second << "\r\n";
	response << "\r\n";
	response << _body;
	return response.str();
}

// Private Methods

bool	Response::_preparation( void )
{
	_loc = _findLocation();
	if (!_loc)
	{
		_setStatus(404, "Not Found");
		_setBody("No matching location");
		return false;
	}
	const std::map<int, std::string> & redir = _loc->getReturn();
	if (!redir.empty())
	{
		int code = redir.begin()->first;
		std::string locationUrl = redir.begin()->second;
		_setStatus(code, (code == 301 ? "Moved Permanently" : "Found"));
		_setHeader("Location", locationUrl);
		return false;
	}
	if (!allowsMethod(_req->getMethod()))
	{
		_setStatus(405, "Method Not Allowed");
		_setBody("Method Not Allowed");
		const std::vector<std::string> & methods = _loc->getMethods();
		std::string allowHeader;
		for (size_t i = 0; i < methods.size(); ++i)
		{
			allowHeader += methods[i];
			if (i != methods.size() - 1)
				allowHeader += ", ";
		}
		_setHeader("Allow", allowHeader);
		return false;
	}
	return true;
}

void	Response::_reconstitution( void ) const
{
	/*if (req.getMethod() == "DELETE")
	{
		if (loc->getPath() != "/upload")
		{
			setStatus(403, "Forbidden");
			setHeader("Content-Type", "application/json");
			setBody("{\"status\":\"error\",\"message\":\"DELETE allowed only in /upload\"}");
			return;
		}
		std::string filePath = resolvePath(loc, req.getPath());
		int status = acstat_file(filePath.c_str(), F_OK | R_OK);
		if (status == 1)
		{
			if (std::remove(filePath.c_str()) == 0)
			{
				setStatus(200, "OK");
				setHeader("Content-Type", "application/json");
				setBody("{\"status\":\"deleted\",\"path\":\"" + req.getPath() + "\"}");
			}
			else
			{
				setStatus(500, "Internal Server Error");
				setHeader("Content-Type", "application/json");
				setBody("{\"status\":\"error\",\"message\":\"Failed to delete file\"}");
			}
			return;
		}
		else if (status == 2)
		{
			setStatus(405, "Method Not Allowed");
			setHeader("Content-Type", "application/json");
			setBody("{\"status\":\"error\",\"message\":\"Cannot delete directory\"}");
			return;
		}
		else
		{
			setStatus(404, "Not Found");
			setHeader("Content-Type", "application/json");
			setBody("{\"status\":\"error\",\"message\":\"File not found\"}");
			return;
		}
	}
	if (req.getMethod() == "POST")
	{
		if (req.getRequestBody().size() > server->getMaxSize())
		{
			setStatus(413, "Payload Too Large");
			setBody("Failed to save uploaded file: > 1M");
			req.discardBody();
			return;
		}
		handlePost(req, loc);
		return;
	}*/
	std::string filePath = resolvePath(loc, req.getPath());
	int status = acstat_file(filePath.c_str(), F_OK | R_OK);
	if (status == 1)
	{
		std::string ext;
		size_t dotPos = filePath.rfind('.');
		if (dotPos != std::string::npos)
			ext = filePath.substr(dotPos);
		const std::map<std::string, std::string> &cgiMap = loc->getCgi();
		std::map<std::string, std::string>::const_iterator it = cgiMap.find(ext);
		if (it != cgiMap.end())
		{
			executeCGI(req, *loc, filePath, *server);
			return;
		}
		if (req.getMethod() == "GET")
		{
			setStatus(200, "OK");
			setBody(readFile(filePath));
			return;
		}
		setStatus(405, "Method Not Allowed");
		setBody("POST not allowed on static file");
		return;
	}
	else if (status == 2)
	{
		std::cout << "filePath in Directory: " << filePath << std::endl;
		if (filePath[filePath.size() - 1] != '/')
			filePath += '/';
		const std::vector<std::string> &indexList = loc->getIndex();
		std::string root = loc->getRoot();
		for (std::vector<std::string>::const_iterator it = indexList.begin(); it != indexList.end(); ++it)
		{
			std::string indexPath = combineIndexPath(filePath, *it, root);
			int s = acstat_file(indexPath.c_str(), F_OK | R_OK);
			if (s == 1)
			{
				setStatus(200, "OK");
				setBody(readFile(indexPath));
				setHeader("Content-Type", getContentType(indexPath));
				return;
    			}
		}
		if (loc->getAutoindex())
		{
			setHeader("Content-Type", "text/html");
			setStatus(200, "OK");
			setBody(generateDirectoryListing(filePath, req.getPath()));
			return;
		}
		setStatus(404, "Not Found");
		const std::map<int, std::string> &errPages = server->getErrorPages();
		if (errPages.find(404) != errPages.end())
		{
			setBody(readFile(errPages.find(404)->second));
			setHeader("Content-Type", "text/html");
		}
		else
			setBody("Directory exists but no index file, File not found");
		return;
	}
	setStatus(404, "Not Found");
	const std::map<int, std::string> &errPages = server->getErrorPages();
	if (errPages.find(404) != errPages.end())
	{
		setBody(readFile(errPages.find(404)->second));
		setHeader("Content-Type", "text/html");
	}
	else
		setBody("Path does not exist, File not found");
}

// utils

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

bool	Response::allowsMethod( const std::string & method ) const
{
	const std::vector<std::string> & methods = _loc->getMethods();
	for (size_t i = 0; i < methods.size(); ++i)
		if (methods[i] == method)
			return true;
	return false;
}

// Setters

void Response::_setStatus( int status, const std::string & statusText ) { _status.first = status; _status.second = statusText; }
void Response::_setBody( const std::string & body ) { _body = body; }
void Response::_setHeader( const std::string & key, const std::string & value ) { _headers[key] = value; }
