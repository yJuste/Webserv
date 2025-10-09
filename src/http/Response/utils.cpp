// ************************************************************************** //
//                                                                            //
//                utils.cpp                                                   //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Response.hpp"
# include "Request.hpp"
# include "Client.hpp"

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
		_status.first = code;
		_status.second = parts[1];
	}
	if (!parts[2].empty() && !parts[3].empty())
		_headers[parts[2]] = parts[3];
	if (!body.empty() && body[0] == '{')
	{
		_body = body;
		_headers["Content-Type"] = "application/json";
		return;
	}
	const std::map<int, std::string>::const_iterator it = _server->getErrorPages().find(_status.first);
	if (it != _server->getErrorPages().end())
	{
		_body = readFile(it->second);
		if (!_body.empty())
		{
			_headers["Content-Type"] = getExtension(it->second);
			return ;
		}
	}
	_body = body;
	_headers["Content-Type"] = getExtension(".txt");
}

const Location *	Response::_findLocation( const std::string & path ) const
{
	const std::vector<Location> & locations = _server->getLocations();
	const Location * best = NULL;
	const Location * root = NULL;
	for (size_t i = 0; i < locations.size(); ++i)
	{
		const std::string & locPath = locations[i].getPath();
		size_t lsize = locPath.size();
		size_t psize = path.size();
		if (locPath == "/")
		{
			root = &locations[i];
			continue;
		}
		if (lsize <= psize && path.compare(0, lsize, locPath) == 0)
			if (psize == lsize || path[lsize] == '/')
				if (!best || lsize > best->getPath().size())
					best = &locations[i];
	}
	if (best)
		return best;
	if (path == "/" && root)
		return root;
	return NULL;
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
		std::string generate = generateDirectoryListing(reducedPath, _req->getPath());
		if (generate == "<html><body><h1>403 Forbidden</h1></body></html>")
			_response("403\nForbidden\n\n\n" + generate);
		else
			_response("200\nOK\nContent-Type\ntext/html\n" + generate);
		return true;
	}
	return _response("404\nNot Found\n\n\nFile not found."), false;
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
	Print::enval(RESET, "     | File", RESET, "[" + rounded(static_cast<size_t>(size)) + std::string(RESET) + "]");
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

std::vector<std::string>	Response::_buildCgiEnv( const std::string & filePath )
{
	std::string locationPath = _loc->getPath();
	std::string fullPath = _req->getPath();
	std::string scriptName;
	std::string pathInfo;
	if (fullPath.find(locationPath) == 0)
	{
		std::string relative = fullPath.substr(locationPath.size());
		size_t scriptEnd = relative.find('/', 1);
		if (scriptEnd != std::string::npos)
		{
			scriptName = locationPath + relative.substr(0, scriptEnd);
			pathInfo = relative.substr(scriptEnd);
		}
		else
		{
			scriptName = fullPath;
			pathInfo = "";
		}
	}
	std::vector<std::string> env;
	env.push_back("SCRIPT_NAME=" + scriptName);
	env.push_back("PATH_INFO=" + pathInfo);
	env.push_back("SCRIPT_FILENAME=" + filePath);
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("REQUEST_METHOD=" + _req->getMethod());
	env.push_back("QUERY_STRING=" + _req->getQuery());
	std::ostringstream oss;
	oss << _req->getBody().size();
	env.push_back("CONTENT_LENGTH=" + oss.str());
	std::string contentType = _req->getHeader("Content-Type");
	if (contentType.empty())
		contentType = "text/html; charset=utf-8";
	env.push_back("CONTENT_TYPE=" + contentType);
	env.push_back("SERVER_NAME=" + _req->getHeader("Host"));
	const std::vector<int> & ports = _server->getEveryPort();
	std::ostringstream ss;
	for (size_t i = 0; i < ports.size(); ++i)
	{
		if (i != 0)
			ss << ",";
		ss << ports[i];
	}
	env.push_back("SERVER_PORT=" + ss.str());
	return env;
}

std::string	Response::_extract_cgi( const std::string & fullPath )
{
	const std::map<std::string, std::string> & cgiMap = _loc->getCgi();
	size_t bestPos = std::string::npos;
	size_t bestLen = 0;
	for (std::map<std::string, std::string>::const_iterator it = cgiMap.begin(); it != cgiMap.end(); ++it)
	{
		const std::string & ext = it->first;
		size_t extPos = fullPath.find(ext);
		if (extPos != std::string::npos && (bestPos == std::string::npos || extPos < bestPos))
		{
			bestPos = extPos;
			bestLen = ext.size();
		}
	}
	if (bestPos != std::string::npos)
		return fullPath.substr(0, bestPos + bestLen);
	return fullPath;
}

void	Response::_apply_session_parameter( void )
{
	replaceAll(_body, "{{session_set_background_color}}", _session->getBgColor());
	std::ostringstream oss;
	oss << _session->getCounter();
	replaceAll(_body, "{{session_set_counter}}", oss.str());
}
