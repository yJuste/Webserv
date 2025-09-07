// ************************************************************************** //
//                                                                            //
//                Request.cpp                                                 //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Layang                                        //
//                                                                            //
// ************************************************************************** //

# include "Request.hpp"

Request::Request() : _headerPart(""), _body(""), _method(""), _path(""), _version(""), _unchunked(false), _printed(false) {}
Request::~Request() {}

Request::Request( const Request & r ) { *this = r; }

Request	& Request::operator = ( const Request & r )
{
	if (this != &r)
	{
		_headerPart = r.getHeaderPart();
		_body = r.getBody();
		_method = r.getMethod();
		_path = r.getPath();
		_version = r.getVersion();
		_headers = r.getHeaders();
		_unchunked = r.getUnchunked();
		_printed = r.getPrinted();
	}
	return *this;
}

void	Request::parse( const std::string & raw )
{
	size_t headerEnd = raw.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return ;

	_headerPart = raw.substr(0, headerEnd);
	_body = raw.substr(headerEnd + 4);

	std::stringstream request(_headerPart);
	request >> _method >> _path >> _version;
	_version.erase(_version.find_last_not_of(" \t\r\n") + 1);

	std::string line;
	std::getline(request, line);
	while (std::getline(request, line))
	{
		size_t colon = line.find(':');
		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);
		value.erase(value.find_last_not_of(" \t\r\n") + 1);
		_headers[key] = value;
	}
}

bool	Request::isComplete( void )
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

void	Request::reset( void )
{
	_headerPart.clear();
	_body.clear();
	_method.clear();
	_path.clear();
	_version.clear();
	_headers.clear();
	_unchunked = false;
	_printed = false; 
}

// Private methods

std::string	Request::_unchunkBody( const std::string & raw )
{
	std::string result;
	size_t pos = 0;

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
		result.append(raw.substr(pos, chunkSize));
		pos += chunkSize + 2;
	}
	return result;
}

// Getters

const std::string & Request::getHeaderPart() const { return _headerPart; }
const std::string & Request::getBody() const { return _body; }
const std::string & Request::getMethod() const { return _method; }
const std::string & Request::getPath() const { return _path; }
const std::string & Request::getVersion() const { return _version; }
const std::map<std::string, std::string> & Request::getHeaders() const { return _headers; }
bool Request::getUnchunked() const { return _unchunked; }
bool Request::getPrinted() const { return _printed; }

// Setter

void Request::setPrinted( bool printed ) { _printed = printed; }
