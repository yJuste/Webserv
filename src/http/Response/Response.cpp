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

Response::Response() : _req(NULL), _server(NULL), _client(NULL), _loc(NULL), _smanager(NULL), _status(200, "OK"), _body("") {}
Response::~Response() {}

Response::Response( Request * req ) : _req(req), _loc(NULL), _status(200, "OK"), _body("")
{
	_client = _req->getClient();
	_server = _client->getServer();
	_smanager = _client->getSessionManager();
}

Response::Response( const Response & r ) { *this = r; }

Response & Response::operator = ( const Response & r )
{
	if (this != &r)
	{
		_req = r._req;
		_server = r._server;
		_client = r._client;
		_loc = r._loc;
		_smanager = r._smanager;
		_status = r._status;
		_headers = r._headers;
		_body = r._body;
	}
	return *this;
}

// Methods

void	Response::build( void )
{
	_check_keep_alive();
	if (_session_management() == 1)
		return ;

	int code = _preparation();
	if (code)
		_reconstitution();
	if (code == 301)
		_response("301\nMoved Permanently\n\n\n");
}

std::string	Response::string( void ) const
{
	std::stringstream response;
	response << "HTTP/1.1 " << _status.first << " " << _status.second << "\r\n";
	if (_headers.find("Content-Type") == _headers.end())
	{
		std::string type = getContentType(_req->getPath());
		response << "Content-Type: " << type << "\r\n";
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

int	Response::_preparation( void )
{
	_loc = _findLocation(_req->getPath());
	if (!_loc)
		return _handleGet(concatPaths(my_getcwd() + "/", _server->getIndex()[0])), 0;

	const std::map<int, std::string> & redir = _loc->getReturn();
	if (!redir.empty())
	{
		std::string locationUrl = redir.begin()->second;
		_loc = _findLocation(locationUrl);
		if (_loc)
		{
			_headers["Location"] = locationUrl;
			if (redir.begin()->first == 301)
				return 301;
		}
		return _404_error("Redirect not found."), 0;
	}
	if (!_allowsMethod(_req->getMethod()))
	{
		std::string allowHeader;
		const std::vector<std::string> & methods = _loc->getMethods();
		_response("405\nMethod Not Allowed\n\n\nMethod Not Allowed");
		for (size_t i = 0; i < methods.size(); ++i)
		{
			allowHeader += methods[i];
			if (i != methods.size() - 1)
				allowHeader += ", ";
		}
		return _headers["Allow"] = allowHeader, 0;
	}
	return 200;
}

void	Response::_reconstitution( void )
{
	std::string filePath = concatPaths(my_getcwd() + "/" + _loc->getRoot(), remove_sub_string(_req->getPath(), _loc->getPath()));
	if (_req->getMethod() == "DELETE")
		return _handleDelete(filePath);
	if (_req->getMethod() == "POST")
		return _handlePost(filePath);
	if (_req->getMethod() == "GET")
		return _handleGet(filePath);
	//return _response("500\nInternal Server Error\n\n\nReconstitution failed.");
}

/*
 *	GET
 */

void	Response::_handleGet( const std::string & path )
{
	int status = acstat(path.c_str(), F_OK | R_OK);
	if (status == 1)
	{
		if (_loc)
		{
			const std::map<std::string, std::string> & cgiMap = _loc->getCgi();
			std::map<std::string, std::string>::const_iterator it = cgiMap.find(getExtension(path));
			if (it != cgiMap.end())
				return _executeCGI(path);
		}
		_response("200\nOK\nContent-Type\n" + getContentType(path) + "\n" + readFile(path));
		return _apply_session_parameter();
	}
	else if (status == 2)
	{
		std::string index = concatPaths(path, _loc->getIndex()[0]);
		if (_autoIndex(index))
			return ;
		if (acstat(index.c_str(), F_OK) == -1)
			return _response("403\nForbidden\n\n\nFile does not exist, forbidden path. ( you should add a default file, like 'index.html')");
		_response("200\nOK\nContent-Type\n" + getContentType(index) + "\n" + readFile(index));
		return _apply_session_parameter();
	}
	else
		return _response("403\nForbidden\n\n\nFile does not exist, forbidden path.");
}

/*
 *	POST
 */

void	Response::_handlePost( const std::string & path )
{
	if (_req->getBody().size() > _server->getMaxSize())
	{
		std::stringstream ss;
		ss << "Failed to save uploaded file: > " << rounded(_server->getMaxSize());
		return _response("413\nPayload Too Large\n\n\n" + ss.str());
	}

	std::string filePath = path;
	std::string contentType = _req->getHeader("Content-Type");
	if (contentType.find("multipart/form-data") != std::string::npos
		|| contentType.find("application/octet-stream") != std::string::npos
		|| contentType.find("text/plain") != std::string::npos
		|| _req->getHeader("Transfer-Encoding") == "chunked")
		return _handleUpload(filePath, contentType);
	if (contentType.find("application/x-www-form-urlencoded") != std::string::npos)
		return _registry(contentType);
	return _404_error("POST route not found.");
}

void	Response::_handleUpload( std::string & filePath, std::string & contentType )
{
	if (contentType.find("multipart/form-data") != std::string::npos)
	{
		bool success = _saveUploadedFile();
		if (success)
		{
			if (_autoIndex(filePath))
				return ;
			return _response("201\nCreated\n\n\nFile uploaded successfully.");
		}
		return _response("500\nInternal Server Error\n\n\nFailed to save uploaded file");
	}
	else if (contentType.find("application/octet-stream") != std::string::npos
		|| contentType.find("text/plain") != std::string::npos
		|| _req->getHeader("Transfer-Encoding") == "chunked")
	{
		const std::vector<char> & data = _req->getBody();
		std::fstream out((_loc->getUpload() + "/upload.bin").c_str(), std::ios::out | std::ios::binary);
		if (out)
		{
			if (!data.empty())
				out.write(&data[0], data.size());
			out.close();
			if (_autoIndex(filePath))
				return ;
			return _response("201\nCreated\n\n\nRaw file uploaded successfully.");
		}
		return _response("500\nInternal Server Error\n\n\nFailed to write file.");
	}
	return _response("415\nUnsupported Media Type\n\n\nContent-Type not supported for upload.");
}

void	Response::_registry( std::string & contentType )
{
	const std::vector<char> & binBody = _req->getBody();
	std::string body;
	if (!binBody.empty())
		body.assign(binBody.begin(), binBody.end());
	std::string username = registryKey(body, "username");
	std::string password = registryKey(body, "password");
	std::string email = registryKey(body, "email");
	if (contentType.find("application/x-www-form-urlencoded") != std::string::npos
		&& body.find("email=") != std::string::npos)
	{
		bool success = _saveUser(username, password, email);
		if (success)
			return _response("201\nCreated\n\n\nAccount created successfully.");
		return _response("400\nBad Request\n\n\nUsername already exists or failed to save.");
	}
	if (contentType.find("application/x-www-form-urlencoded") != std::string::npos)
	{
		if (_checkUser(username, password))
			return _response("200\nOK\n\n\nLogin successful.");
		return _response("401\nUnauthorized\n\n\nInvalid username or password.");
	}
	return _404_error("POST route not found.");
}

/*
 *	DELETE
 */

void	Response::_handleDelete( std::string & path )
{
	if (_loc->getPath().find("upload") == std::string::npos)
		return _response("403\nForbidden\nContent-Type\napplication/json\n{\"status\":\"error\",\"message\":\"DELETE allowed only in /upload\"}");

	path = concatPaths(path, _req->getPath());
	int status = acstat(path.c_str(), F_OK | R_OK);
	if (status == 1)
	{
		if (std::remove(path.c_str()) == 0)
			return _response("200\nOK\nContent-Type\napplication/json\n{\"status\":\"deleted\",\"path\":\"" + remove_sub_string(path, my_getcwd() + "/") + "\"}");
		return _response("500\nInternal Server Error\nContent-Type\napplication/json\n{\"status\":\"error\",\"message\":\"Failed to delete file\"}");
	}
	else if (status == 2)
		return _response("405\nMethod Not Allowed\nContent-Type\napplication/json\n{\"status\":\"error\",\"message\":\"Cannot delete directory\"}");
	return _response("404\nDELETE found nothing.\nContent-Type\napplication/json\n{\"status\":\"error\",\"message\":\"File not found\"}");
}

/*
 *	CGI
 */

void	Response::_executeCGI( const std::string & filePath )
{
	int sv[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0)
		return _response("500\nInternal Server Error\n\n\nFailed to create socketpair CGI");
	pid_t pid = fork();
	if (pid < 0)
	{
		close(sv[0]);
		close(sv[1]);
		return _response("500\nInternal Server Error\n\n\nFailed to fork() CGI process.");
	}
	if (pid == 0)
	{
        	signal(SIGPIPE, SIG_IGN);
		close(sv[0]);
		dup2(sv[1], STDIN_FILENO);
		dup2(sv[1], STDOUT_FILENO);
		close(sv[1]);

		std::string ext;
		size_t dotPos = filePath.rfind('.');
		if (dotPos != std::string::npos)
			ext = filePath.substr(dotPos);

		const std::map<std::string, std::string> & cgiMap = _loc->getCgi();
		std::map<std::string, std::string>::const_iterator it = cgiMap.find(ext);
		if (it == cgiMap.end())
			kill(getpid(), SIGTERM);

		const char * cgiPath = it->second.c_str();
		const char * scriptPath = filePath.c_str();
		const char * argv[] = { cgiPath, scriptPath, NULL };
		std::vector<std::string> env = _buildCgiEnv(filePath);
		std::vector<char *> envp;
		std::vector<char *> buffers;
		for (size_t i = 0; i < env.size(); ++i)
			envp.push_back(const_cast<char *>(env[i].c_str()));
		envp.push_back(NULL);
		execve(cgiPath, (char * const *)argv, &envp[0]);
		kill(getpid(), SIGTERM);
	}
	close(sv[1]);
	if (_status.first == 200)
		_client->setSv(sv[0]);
}

/*
 *	SESSION
 */

int	Response::_session_management( void )
{
	std::string sid = _smanager->getSessionIdFromCookie(_req->getHeader("Cookie"));
	_session = _smanager->getSession(sid);
	if (!_session)
	{
		sid = _smanager->create();
		std::string value = "session_id=" + sid + "; Max-Age=3600; HttpOnly; SameSite=Strict";
		_headers["Set-Cookie"] = value;
		_session = _smanager->getSession(sid);
		return 0;
	}
	if (_req->getPath() == "/session_set_background_color")
	{
		const std::vector<char> & body = _req->getBody();
		std::string data(body.begin(), body.end());
		_session->setBgColor(url_decode(data.substr(6)));
		_response("200\nOK\nContent-Type\ntext/plain\nColor has changed.");
		return 1;
	}
	if (_req->getPath() == "/")
		_session->incrementCounter();
	return 0;
}

void	Response::_apply_session_parameter( void )
{
	replaceAll(_body, "{{session_set_background_color}}", _session->getBgColor());
	std::ostringstream oss;
	oss << _session->getCounter();
	std::string counter = oss.str();
	replaceAll(_body, "{{session_set_counter}}", counter);
}

/*
 *	KEEP_ALIVE
 */

void	Response::_check_keep_alive( void )
{
	std::string connexion = _req->getHeader("Connection");
	if (connexion != "")
	{
		if (connexion == "close")
			_headers["Connection"] = "close";
		else
			_headers["Connection"] = "keep-alive";
	}
}

// Getters

const std::pair<int, std::string> & Response::getStatus() const { return _status; }
const std::map<std::string, std::string> & Response::getHeaders() const { return _headers; }
std::string Response::getHeader( const std::string & type ) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(type);
	if (it != _headers.end()) return it->second;
	return "";
}
const std::string & Response::getBody() const { return _body; }
