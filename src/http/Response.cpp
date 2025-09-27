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

Response::Response( const Response & r ) { *this = r; }

Response & Response::operator = ( const Response & r )
{
	if (this != &r)
	{
		_req = r._req;
		_server = r._server;
		_client = r._client;
		_loc = r._loc;
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
	response << "\r\n";
	response << _body;
	return response.str();
}

// Private Methods

int	Response::_preparation( void )
{
	_loc = _findLocation(_req->getPath());
	if (!_loc)
		return _404_error("No matching location."), 0;

	const std::map<int, std::string> & redir = _loc->getReturn();
	if (!redir.empty())
	{
		std::string locationUrl = redir.begin()->second;
		_loc = _findLocation(locationUrl);
		if (_loc)
		{
			_setHeader("Location", locationUrl);
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
		return _setHeader("Allow", allowHeader), 0;
	}
	return 200;
}

void	Response::_reconstitution( void )
{
	std::string filePath = _resolvePath();
	if (_req->getMethod() == "DELETE")
		return _handleDelete(filePath);
	if (_req->getMethod() == "POST")
		return _handlePost(filePath);

	int status = acstat(filePath.c_str(), F_OK | R_OK);
	if (status == 1)
	{
		const std::map<std::string, std::string> & cgiMap = _loc->getCgi();
		std::map<std::string, std::string>::const_iterator it = cgiMap.find(getExtension(filePath));
		if (it != cgiMap.end())
			return _executeCGI(filePath);
		if (_req->getMethod() == "GET")
			return _response("200\nOK\nContent-Type\n" + getContentType(filePath) + "\n" + readFile(filePath));
		return _response("405\nMethod Not Allowed\n\n\nPost not allowed on static file.");
	}
	else if (status == 2)
	{
		std::string index = concatPaths(filePath, _loc->getIndex()[0]);
		if (_autoIndex(index))
			return ;
		if (acstat(index.c_str(), F_OK) == -1)
			return _response("403\nForbidden\n\n\nFile does not exist, forbidden path. ( you should add a default file, like 'index.html')");
		return _response("200\nOK\nContent-Type\n" + getContentType(index) + "\n" + readFile(index));
	}
	return _404_error("Path does not exist, File not found.");
}

/*
 *	POST
 */

void	Response::_handlePost( const std::string & path )
{
	if (_req->getBody().size() > _server->getMaxSize())
	{
		std::stringstream ss;
		ss << "Failed to save uploaded file: > " << rounded(_server->getMaxSize()) << " bytes";
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

std::vector<std::string>	Response::_buildCgiEnv( const std::string & filePath )
{
	std::vector<std::string> env;
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("REQUEST_METHOD=" + _req->getMethod());
	env.push_back("SCRIPT_FILENAME=" + filePath);
	env.push_back("SCRIPT_NAME=" + _req->getPath());
	env.push_back("QUERY_STRING=" + _req->getQuery());

	std::ostringstream oss;
	oss << _req->getBody().size();
	env.push_back("CONTENT_LENGTH=" + oss.str()); 
	env.push_back("CONTENT_TYPE=" + _req->getHeader("Content-Type"));
	env.push_back("SERVER_NAME=" + _req->getHeader("Host"));

	const std::vector<int> & ports = _server->getEveryPort();
	std::ostringstream ss;
	for (size_t i = 0; i < ports.size(); ++i)
	{
		if (i != 0) ss << ",";
			ss << ports[i];
	}
	env.push_back("SERVER_PORT=" + ss.str());
	return env;
}

void	Response::_executeCGI( const std::string & filePath )
{
	int inPipe[2];
	int outPipe[2];
	if (pipe(inPipe) < 0 || pipe(outPipe) < 0)
		return _response("500\nInternal Server Error\n\n\nFailed to create pipes for CGI");
	pid_t pid = fork();
	if (pid < 0)
	{
		close(inPipe[0]); close(inPipe[1]);
		close(outPipe[0]); close(outPipe[1]);
		return _response("500\nInternal Server Error\n\n\nFailed to fork() CGI process.");
	}
	if (pid == 0)
	{
        	signal(SIGPIPE, SIG_IGN);
		close(inPipe[1]);
		close(outPipe[0]);
		dup2(inPipe[0], 0);
		dup2(outPipe[1], 1);
		close(inPipe[0]);
		close(outPipe[1]);

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
		std::vector<char*> envp;
		std::vector<char*> buffers;
		for (size_t i = 0; i < env.size(); ++i)
		{
			char * copy = new char[env[i].size() + 1]; 
			std::strcpy(copy, env[i].c_str());
			envp.push_back(copy);
			buffers.push_back(copy);
		}
		envp.push_back(NULL);
		execve(cgiPath, (char * const *)argv, (char * const *)&envp[0]);
		for (size_t i = 0; i < buffers.size(); ++i)
			delete [] buffers[i];
		kill(getpid(), SIGTERM);
	}
	close(inPipe[0]); 
	close(outPipe[1]);
	if (_req->getMethod() == "POST")
	{
		const std::vector<char> & body = _req->getBody();
		ssize_t total = 0;
		while (total < (ssize_t)body.size())
		{
			ssize_t w = write(inPipe[1], &body[total], _body.size() - total);
			if (w <= 0)
				break ;
			total += w;
		}
	}
	close(inPipe[1]);

	char buffer[4096];
	std::string cgiOutput;
	ssize_t n;
	while ((n = read(outPipe[0], buffer, sizeof(buffer))) > 0)
		cgiOutput.append(buffer, n);
	close(outPipe[0]);

	int status;
	waitpid(pid, &status, 0);
	std::string rawHeaders;
	std::string body;
	size_t headerEndPos = std::string::npos;
	size_t rnPos = cgiOutput.find("\r\n\r\n");
	size_t nPos  = cgiOutput.find("\n\n");
	if (rnPos != std::string::npos)
		headerEndPos = rnPos;
	else if (nPos != std::string::npos)
		headerEndPos = nPos;
	if (headerEndPos != std::string::npos)
	{
		rawHeaders = cgiOutput.substr(0, headerEndPos);
		size_t bodyStart = (headerEndPos == rnPos) ? headerEndPos + 4 : headerEndPos + 2;
		body = cgiOutput.substr(bodyStart);
		std::istringstream headerStream(rawHeaders);
		std::string line;
		while (std::getline(headerStream, line))
		{
			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1, 1);

			size_t colon = line.find(':');
			if (colon != std::string::npos)
			{
				std::string key = line.substr(0, colon);
				std::string value = line.substr(colon + 1);
				if (!value.empty() && value[0] == ' ')
					value.erase(0, 1);
				_headers[key] = value;
			}
		}
	}
	else
		body = cgiOutput;
	if (!_headers.count("Content-Type"))
		_headers["Content-Type"] = "text/html";
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it)
		_setHeader(it->first, it->second);
	_setBody(body);
	if (!_headers.count("Status"))
		return _setStatus(200, "OK");
	std::istringstream ss(_headers["Status"]);
	int code;
	std::string text;
	ss >> code;
	std::getline(ss, text);
	_setStatus(code, text);
}

/*
 *	UTILS
 */

void	Response::_response( const std::string & input )
{
	std::vector<std::string> parts;
	std::string current;
	size_t i = 0;
	for (; i < input.size(); ++i)
	{
		if (input[i] == '\n')
		{
			parts.push_back(current);
			current.clear();
			if (parts.size() == 4)
			{
				++i;
				break;
			}
		}
		else
			current += input[i];
	}
	if (!current.empty() && parts.size() < 4)
	{
		parts.push_back(current);
		current.clear();
	}
	while (parts.size() < 4)
		parts.push_back("");

	std::string body;
	if (i < input.size())
		body = input.substr(i);
	if (!parts[0].empty())
	{
		int code = 0;
		std::istringstream(parts[0]) >> code;
		_setStatus(code, parts[1]);
	}
	if (!parts[2].empty() && !parts[3].empty())
		_setHeader(parts[2], parts[3]);
	if (!body.empty())
		_setBody(body);
}

std::string	Response::_resolvePath( void )
{
	std::string filePath = _loc->getRoot();
	if (_loc->getPath() == _req->getPath())
	{
		if (filePath.size() >= 2 && filePath[0] == '.' && filePath[1] == '/')
			filePath = my_getcwd() + "/" + filePath;
		return filePath;
	}
	return concatPaths(my_getcwd() + "/" + filePath, _req->getPath());
}

void	Response::_404_error( const std::string & status )
{
	const std::map<int, std::string> & errPages = _server->getErrorPages();
	_setStatus(404, "Not Found");
	if (errPages.find(404) != errPages.end())
		_setBody(readFile(errPages.find(404)->second));
	else
		_setBody(status);
	_setHeader("Content-Type", getExtension(errPages.find(404)->second));
}

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
	const std::vector<Location> &locations = _server->getLocations();
	const Location *best = NULL;
	for (size_t i = 0; i < locations.size(); ++i)
	{
		const std::string &locPath = locations[i].getPath();
		size_t lsize = locPath.size();
		size_t psize = path.size();
		if (lsize <= psize && path.compare(0, lsize, locPath) == 0)
		{
			if (locPath == "/")
			{
				if (!best)
					best = &locations[i];
				continue;
			}
			if (psize == lsize || path[lsize] == '/')
				if (!best || lsize > best->getPath().size())
					best = &locations[i];
		}
	}
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

bool	Response::_autoIndex( const std::string & path )
{
	if (_loc->getAutoindex())
	{
		std::string reducedPath = path;
		while (!reducedPath.empty() && reducedPath[reducedPath.size() - 1] != '/')
			reducedPath.erase(reducedPath.size() - 1);
		_response("200\nOK\nContent-Type\ntext/html\n" + generateDirectoryListing(reducedPath, _req->getPath()));
		return true;
	}
	return false;
}

bool	Response::_saveUploadedFile( void )
{
	const std::vector<char> & binBody = _req->getBody();
	if (binBody.empty())
		return false;

	std::string body(binBody.begin(), binBody.end());
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

	std::string fullPath = _loc->getUpload() + "/" + filename;
	std::ofstream out(fullPath.c_str(), std::ios::binary | std::ios::trunc);
	if (!out)
		return false;
	if (contentEnd > contentStart)
		out.write(&binBody[contentStart], contentEnd - contentStart);
	out.close();

	std::ifstream in(fullPath.c_str(), std::ios::binary | std::ios::ate);
	std::streamsize size = in.tellg();
	in.close();
	Print::enval(RESET, "    | File", RESET, "[" + rounded(static_cast<size_t>(size)) + std::string(RESET) + "]");
	return true;
}

bool	Response::_saveUser( const std::string & username, const std::string & password, const std::string & email )
{
	std::ofstream create(USERS_FILE, std::ios::app);
	if (!create)
		return false;
	create.close();

	bool exists = false;
	std::ifstream file1(USERS_FILE, std::ifstream::in);
	if (!file1)
		return false;

	std::string line;
	while (std::getline(file1, line))
	{
		size_t sep = line.find(':');
		if (sep != std::string::npos)
		{
			std::string user = line.substr(0, sep);
			if (user == username)
				exists = true;
		}
	}
	if (exists)
		return false;

	std::ofstream file2(USERS_FILE, std::ios::app);
	if (!file2)
		return false;
	file2 << username << ":" << password << ":" << email << std::endl;
	file2.close();
	return true;
}

bool	Response::_checkUser( const std::string & username, const std::string & password ) const
{
	std::ofstream create(USERS_FILE, std::ios::app);
	if (!create)
		return false;
	create.close();

	std::ifstream file(USERS_FILE, std::ifstream::in);
	if (!file)
		return false;

	std::string line;
	while (std::getline(file, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		std::istringstream iss(line);
		std::string user, pass, email;
		if (std::getline(iss, user, ':')
			&& std::getline(iss, pass, ':')
			&& std::getline(iss, email))
			if (user == username && pass == password)
				return true;
	}
	return false;
}

// Getters

const std::pair<int, std::string> & Response::getStatus() const { return _status; }
const std::map<std::string, std::string> & Response::getHeaders() const { return _headers; }
std::string Response::getHeader( const std::string & type ) const { std::map<std::string, std::string>::const_iterator it = _headers.find(type); if (it != _headers.end()) return it->second; return ""; }
const std::string & Response::getBody() const { return _body; }

// Setters

void Response::_setStatus( int status, const std::string & statusText ) { _status.first = status; _status.second = statusText; }
void Response::_setBody( const std::string & body ) { _body = body; }
void Response::_setHeader( const std::string & key, const std::string & value ) { _headers[key] = value; }
