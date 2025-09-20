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
		return false;
	}
	return true;
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
	/*
	if (_req->getMethod() == "POST")
	{
		if (_req->getBody().size() > _server->getMaxSize())
		{
			_setStatus(413, "Payload Too Large");
			std::stringstream ss;
			ss << "Failed to save uploaded file: > " << rounded(_server->getMaxSize()) << " bytes" << std::endl;
			_setBody(ss.str());
			return ;
		}
		//_handlePost();
		return ;
	}*/
	std::string filePath = _resolvePath(_req->getPath());
	std::cout << filePath << std::endl;
	int status = acstat(filePath.c_str(), F_OK | R_OK);
	if (status == 1)
	{
		std::string ext;
		size_t dotPos = filePath.rfind('.');
		if (dotPos != std::string::npos)
			ext = filePath.substr(dotPos);
		const std::map<std::string, std::string> & cgiMap = _loc->getCgi();
		std::map<std::string, std::string>::const_iterator it = cgiMap.find(ext);
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
		if (acstat(index.c_str(), F_OK) == -1)
		{
			_setStatus(403, "Forbidden");
			_setBody("File does not exist, forbidden path.");
			return ;
		}
		_setStatus(200, "OK");
		_setBody(readFile(index));
		_setHeader("Content-Type", getContentType(index));
		if (_loc->getAutoindex())
		{
			_setStatus(200, "OK");
			_setHeader("Content-Type", "text/html");
			_setBody(generateDirectoryListing(filePath, _req->getPath()));
		}
		return ;
	}
	_setStatus(404, "Not Found");
	const std::map<int, std::string> & errPages = _server->getErrorPages();
	if (errPages.find(404) != errPages.end())
		_setBody(readFile(errPages.find(404)->second));
	else
		_setBody("Path does not exist, File not found");
	_setHeader("Content-Type", errPages.find(404)->second);
}

/*
void HttpResponse::handlePost( void )
{
	std::string path = _req->getPath();
	std::string contentType = _req->getHeader("Content-Type");
	if (!_loc->getUpload().empty()) // upload
	{
		bool success = false;

		if (contentType.find("multipart/form-data") != std::string::npos)
			success = saveUploadedFile(req, loc->getUpload());
		else if (req.getHeader("Transfer-Encoding") == "chunked" ||
				contentType.find("text/plain") != std::string::npos)
		{
			std::cout << "enter in /upload chunked"<< std::endl;
			std::string data = req.getRequestBody();
			std::string filePath = loc->getUpload() + "/chunked_file.txt";

			std::ofstream out(filePath.c_str(), std::ios::binary);
			if (out)
			{
				out.write(data.c_str(), data.size());
				out.close();
				success = true;
			}
		}
		if (success) {
			setStatus(201, "Created");
			setBody("File uploaded successfully");
		} else {
			setStatus(500, "Internal Server Error");
			setBody("Failed to save uploaded file");
		}
		return;
	}

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

	setStatus(404, "Not Found");
	setBody("POST route not found");
}*/

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
	std::cout << "!" << root << std::endl;

	std::string locPath = _loc->getPath();
	std::string path = reqPath;
	std::cout << ":" << locPath << std::endl;
	std::cout << "?" << path << std::endl;

	if (!locPath.empty() && locPath != "/" && path.find(locPath) == 0)
		path = path.substr(locPath.size());
	while (!path.empty() && path[0] == '/')
		path.erase(0, 1);
	std::cout << "?" << path << std::endl;
	if (acstat((_loc->getRoot() + path).c_str(), F_OK | R_OK) == 1)
		return root + "/" + _loc->getRoot() + path;
	return root + "/" + path;
}

// Setters

void Response::_setStatus( int status, const std::string & statusText ) { _status.first = status; _status.second = statusText; }
void Response::_setBody( const std::string & body ) { _body = body; }
void Response::_setHeader( const std::string & key, const std::string & value ) { _headers[key] = value; }
