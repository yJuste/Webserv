// ************************************************************************** //
//                                                                            //
//                Request.cpp                                                 //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Layang                                        //
//                                                                            //
// ************************************************************************** //

# include "Request.hpp"

Request::Request() : _client(NULL), _headerPart(""), _body(""), _method(""), _path(""), _version(""), _query(""), _unchunked(false), _printed(false) {}
Request::~Request() {}

Request::Request( const Client * client ) : _client(client), _headerPart(""), _body(""), _method(""), _path(""), _version(""), _query(""), _unchunked(false), _printed(false) {}

Request::Request( const Request & r ) { *this = r; }

Request	& Request::operator = ( const Request & r )
{
	if (this != &r)
	{
		_client = r.getClient();
		_headerPart = r.getHeaderPart();
		_body = r.getBody();
		_method = r.getMethod();
		_path = r.getPath();
		_version = r.getVersion();
		_query = r.getQuery();
		_headers = r.getHeaders();
		_unchunked = r.getUnchunked();
		_printed = r.getPrinted();
	}
	return *this;
}

int	Request::create( const std::string & raw )
{
	_reset();
	size_t headerEnd = raw.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return 1;

	_headerPart = raw.substr(0, headerEnd);
	_body = raw.substr(headerEnd + 4);

	std::stringstream request(_headerPart);
	request >> _method >> _path >> _version;
	_version.erase(_version.find_last_not_of(" \t\r\n") + 1);
	size_t qpos = _path.find('?');
	if (qpos != std::string::npos)
	{
		_query = _path.substr(qpos + 1);
		_path = _path.substr(0, qpos);
	}

	std::string line;
	std::getline(request, line);
	while (std::getline(request, line))
	{
		size_t colon = line.find(':');
		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);
		value.erase(0, value.find_first_not_of(" \t\r\n"));
		value.erase(value.find_last_not_of(" \t\r\n") + 1);
		_headers[key] = value;
	}
	if (!_isComplete())
		return 2;
	if (!getPrinted())
		setPrinted(true);
	return 0;
}

// Private methods

std::string	Request::_unchunkBody( const std::string & raw )
{
	std::string result;
	size_t pos = 0;

	size_t firstCRLF = raw.find("\r\n");
	if (firstCRLF != std::string::npos)
		pos = firstCRLF + 2;
	else
		pos = 0;
	while (true)
	{
		size_t endOfSize = raw.find("\r\n", pos);
		if (endOfSize == std::string::npos)
			break;
		std::string sizeStr = raw.substr(pos, endOfSize - pos);
		size_t semicolon = sizeStr.find(';');
		if (semicolon != std::string::npos)
			sizeStr = sizeStr.substr(0, semicolon);
		int chunkSize = std::strtol(sizeStr.c_str(), NULL, 16);
		if (chunkSize == 0)
			break;
		pos = endOfSize + 2;
		if (pos + chunkSize > raw.size())
			break;
		std::string chunkData = raw.substr(pos, chunkSize);
		result.append(chunkData);
		pos += chunkSize + 2;
	}
	return result;
}

bool	Request::_isComplete( void )
{
	if (_headers.count("Content-Length"))
	{
		std::stringstream ss(_headers["Content-Length"]);
		size_t len = 0;
		ss >> len;
		return _body.size() >= len;
	}
	if (_headers.count("Transfer-Encoding") && _headers["Transfer-Encoding"] == "chunked")
	{
		if (_body.find("0\r\n\r\n") != std::string::npos)
		{
			if (!_unchunked)
			{
				_body = _unchunkBody(_body);
				_unchunked = true;
			}
			return true;
		}
		return false;
	}
	return true;
}

void	Request::_reset( void )
{
	_headerPart.clear();
	_body.clear();
	_method.clear();
	_path.clear();
	_version.clear();
	_query.clear();
	_headers.clear();
	_unchunked = false;
	_printed = false; 
}

// Getters

const Client * Request::getClient() const { return _client; }
const std::string & Request::getHeaderPart() const { return _headerPart; }
const std::string & Request::getBody() const { return _body; }
const std::string & Request::getMethod() const { return _method; }
const std::string & Request::getPath() const { return _path; }
const std::string & Request::getVersion() const { return _version; }
const std::string & Request::getQuery() const { return _query; }
const std::map<std::string, std::string> & Request::getHeaders() const { return _headers; }
bool Request::getUnchunked() const { return _unchunked; }
bool Request::getPrinted() const { return _printed; }

// Setter

void Request::setPrinted( bool printed ) { _printed = printed; }
