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
	_check_keep_alive();
	int code = _preparation();
	if (code)
		_reconstitution();
	if (code == 301)
		_setStatus(301, "Moved Permanently");
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
	//response << "Set-Cookie: webserv=xyz789; Path=/site/; HttpOnly" << "\r\n";
	response << "\r\n";
	response << _body;
	return response.str();
}

// Private Methods

int	Response::_preparation( void )
{
	_loc = _findLocation();
	if (!_loc)
	{
		_404_error("No matching location");
		return 0;
	}
	const std::map<int, std::string> & redir = _loc->getReturn();
	if (!redir.empty())
	{
		int code = redir.begin()->first;
		std::string locationUrl = redir.begin()->second;
		_loc = _findLocation(locationUrl);
		if (_loc)
		{
			_setHeader("Location", locationUrl);
			if (code == 301)
				return 301;
		}
		_404_error("Redirect not found");
		return 0;
	}
	if (!_allowsMethod(_req->getMethod()))
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
		return 0;
	}
	return 200;
}

void	Response::_reconstitution( void )
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
	}*/
	std::string filePath = _resolvePath(_req->getPath());
	if (_req->getMethod() == "POST")
		return _handlePost(filePath);
	int status = acstat(filePath.c_str(), F_OK | R_OK);
	if (status == 1)
	{
		const std::map<std::string, std::string> & cgiMap = _loc->getCgi();
		std::map<std::string, std::string>::const_iterator it = cgiMap.find(getExtension(filePath));
		if (it != cgiMap.end())
		{
			//executeCGI(req, *loc, filePath, *server);
			return ;
		}
		if (_req->getMethod() == "GET")
		{
			_setStatus(200, "OK");
			_setBody(readFile(filePath));
			_setHeader("Content-Type", getContentType(filePath));
			return ;
		}
		_setStatus(405, "Method Not Allowed");
		_setBody("POST not allowed on static file");
		return ;
	}
	else if (status == 2)
	{
		if (filePath[filePath.size() - 1] != '/')
			filePath += '/';
		std::string index = filePath + _loc->getIndex()[0];
		if (_autoIndex(index))
			return ;
		if (acstat(index.c_str(), F_OK) == -1)
		{
			_setStatus(403, "Forbidden");
			_setBody("File does not exist, forbidden path. ( you should add a default file, like 'index.html')");
			return ;
		}
		_setStatus(200, "OK");
		_setBody(readFile(index));
		_setHeader("Content-Type", getContentType(index));
		return ;
	}
	_404_error("Path does not exist, File not found");
}

void	Response::_handlePost( const std::string & path )
{
	if (_req->getBody().size() > _server->getMaxSize())
	{
		_setStatus(413, "Payload Too Large");
		std::stringstream ss;
		ss << "Failed to save uploaded file: > " << rounded(_server->getMaxSize()) << " bytes" << std::endl;
		_setBody(ss.str());
		return ;
	}

	std::string filePath = path;
	std::string contentType = _req->getHeader("Content-Type");

	if (contentType.find("multipart/form-data") != std::string::npos)
	{
		bool success = _saveUploadedFile();
		if (success)
		{
			if (filePath[filePath.size() - 1] != '/')
				filePath += '/';
			filePath += _loc->getRoot();
			_setStatus(201, "Created");
			_setBody("File uploaded successfully");
			if (_autoIndex(filePath))
				return ;
		}
		else
		{
			_setStatus(500, "Internal Server Error");
			_setBody("Failed to save uploaded file");
		}
		return;
	}
	else if (contentType.find("application/octet-stream") != std::string::npos
		|| contentType.find("text/plain") != std::string::npos
		|| _req->getHeader("Transfer-Encoding") == "chunked")
	{
		std::string data = _req->getBody();
		std::string filePath = _loc->getUpload() + "/upload.bin";
		std::fstream out(filePath.c_str(), std::fstream::binary);
		if (out)
		{
			out.write(data.c_str(), data.size());
			out.close();
			_setStatus(201, "Created");
			_setBody("Raw file uploaded successfully");
		}
		else
		{
			_setStatus(500, "Internal Server Error");
			_setBody("Failed to write file");
		}
		return;
	}
	else
	{
		_setStatus(415, "Unsupported Media Type");
		_setBody("Content-Type not supported for upload");
		return;
	}
	/*
	if (path == "/register" &&
	    contentType.find("application/x-www-form-urlencoded") != std::string::npos)
	{
		std::string username = getFormValue(req.getRequestBody(), "username");
		std::string password = getFormValue(req.getRequestBody(), "password");

		bool success = saveUser(username, password);
		if (success) {
			setStatus(201, "Created");
			setBody("Account created successfully");
		} else {
			setStatus(400, "Bad Request");
			setBody("Username already exists or failed to save");
		}
		return;
	}

	if (path == "/login" &&
	    contentType.find("application/x-www-form-urlencoded") != std::string::npos)
	{
		std::string username = getFormValue(req.getRequestBody(), "username");
		std::string password = getFormValue(req.getRequestBody(), "password");

		if (checkUser(username, password)) {
			setStatus(200, "OK");
			setBody("Login successful");
		} else {
			setStatus(401, "Unauthorized");
			setBody("Invalid username or password");
		}
		return;
	}
	*/
	_setStatus(404, "Not Found");
	_setBody("POST route not found");
	return ;
}

// utils

void	Response::_check_keep_alive( void )
{
	const std::map<std::string, std::string> headers = _req->getHeaders();
	std::map<std::string, std::string>::const_iterator it = headers.find("Connection");
	if (it != headers.end())
	{
		if (it->second == "close")
			_headers["Connection"] = "close";
		else
			_headers["Connection"] = "keep-alive";
	}
}

const Location *	Response::_findLocation( const std::string & path ) const
{
	const std::vector<Location> & locations = _server->getLocations();
	const Location * best = NULL;

	for (size_t i = 0; i < locations.size(); ++i)
	{
		const std::string & locPath = locations[i].getPath();
		if (!locPath.empty() && path.find(locPath) == 0)
			if (!best || locPath.size() > best->getPath().size())
				best = &locations[i];
	}
	return best;
}

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

bool	Response::_allowsMethod( const std::string & method ) const
{
	const std::vector<std::string> & methods = _loc->getMethods();
	for (size_t i = 0; i < methods.size(); ++i)
		if (methods[i] == method)
			return true;
	return false;
}

std::string	Response::_resolvePath( const std::string & reqPath )
{
	std::string root = _loc->getRoot();
	if (root.size() >= 2 && root[0] == '.' && root[1] == '/')
		root = my_getcwd() + "/";
	while (!root.empty() && root[root.size() - 1] == '/')
		root.erase(root.size() - 1);

	std::string locPath = _loc->getPath();
	std::string path = reqPath;

	if (!locPath.empty() && locPath != "/" && path.find(locPath) == 0)
		path = path.substr(locPath.size());
	while (!path.empty() && path[0] == '/')
		path.erase(0, 1);
	if (acstat((_loc->getRoot() + path).c_str(), F_OK | R_OK) == 1)
		return root + "/" + _loc->getRoot() + path;
	return root + "/" + path;
}

void	Response::_404_error( const std::string & body )
{
	_setStatus(404, "Not Found");
	const std::map<int, std::string> & errPages = _server->getErrorPages();
	if (errPages.find(404) != errPages.end())
		_setBody(readFile(errPages.find(404)->second));
	else
		_setBody(body);
	_setHeader("Content-Type", getExtension(errPages.find(404)->second));
}

bool	Response::_autoIndex( const std::string & path )
{
	if (_loc->getAutoindex())
	{
		_setStatus(200, "OK");
		_setHeader("Content-Type", "text/html");
		std::string reducedPath = path;
		while (!reducedPath.empty() && reducedPath.back() != '/')
			reducedPath.pop_back();
		_setBody(generateDirectoryListing(reducedPath, _req->getPath()));
		return true;
	}
	return false;
}

bool	Response::_saveUploadedFile( void )
{
	std::string body = _req->getBody();
	std::string contentType = _req->getHeader("Content-Type");
	size_t pos = contentType.find("boundary=");
	if (pos == std::string::npos)
		return false;
	const std::string boundary = "--" + contentType.substr(pos + 9);
	const std::string closingBoundary = boundary + "--";

	size_t start = body.find(boundary);
	if (start == std::string::npos)
		return false;
	size_t fnStart = body.find("filename=\"", start);
	if (fnStart == std::string::npos)
		return false;
	fnStart += 10;
	size_t fnEnd = body.find("\"", fnStart);
	if (fnEnd == std::string::npos)
		return false;

	std::string filename = body.substr(fnStart, fnEnd - fnStart);
	size_t contentStart = body.find("\r\n\r\n", fnEnd);
	if (contentStart == std::string::npos)
		return false;
	contentStart += 4;
	size_t nextBoundaryPos = body.find(boundary, contentStart);
	if (nextBoundaryPos == std::string::npos)
	{
		nextBoundaryPos = body.find(closingBoundary, contentStart);
		if (nextBoundaryPos == std::string::npos)
			return false;
	}
	if (nextBoundaryPos < 2 || nextBoundaryPos < contentStart)
		return false;
	size_t contentEnd = nextBoundaryPos - 2;
	if (contentEnd < contentStart)
		return false;
	std::string fileContent = body.substr(contentStart, contentEnd - contentStart);
	std::string fullPath = _loc->getUpload() + "/" + filename;
	std::ofstream out(fullPath, std::ios::binary | std::ios::trunc);
	if (!out)
		return false;
	out.write(fileContent.c_str(), fileContent.size());
	out.close();

	std::ifstream in(fullPath.c_str(), std::ios::binary | std::ios::ate);
	std::streamsize size = in.tellg();
	in.close();
	Print::enval(RESET, "    | File", RESET, "[" + rounded(static_cast<size_t>(size)) + " bytes" + std::string(RESET) + "]");
	return true;
}

// Getters

const std::pair<int, std::string> & Response::getStatus() const { return _status; }
const std::map<std::string, std::string> & Response::getHeaders() const { return _headers; }
std::string Response::getHeader( const std::string & type ) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(type);
	if (it != _headers.end())
		return it->second;
	return "";
}
const std::string & Response::getBody() const { return _body; }

// Setters

void Response::_setStatus( int status, const std::string & statusText ) { _status.first = status; _status.second = statusText; }
void Response::_setBody( const std::string & body ) { _body = body; }
void Response::_setHeader( const std::string & key, const std::string & value ) { _headers[key] = value; }
