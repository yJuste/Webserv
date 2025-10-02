// ************************************************************************** //
//                                                                            //
//                Request.cpp                                                 //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Layang                                        //
//                                                                            //
// ************************************************************************** //

# include "Request.hpp"

Request::Request() : _client(NULL), _headerPart(""), _method(""), _path(""), _version(""), _query(""), _rawBuf(""), _unchunked(false), _printed(false) {}
Request::~Request() {}

Request::Request( Client * client ) : _client(client), _headerPart(""), _method(""), _path(""), _version(""), _query(""), _rawBuf(""), _unchunked(false), _printed(false) {}

Request::Request( const Request & r ) { *this = r; }

Request	& Request::operator = ( const Request & r )
{
	if (this != &r)
	{
		_client = r._client;
		_headerPart = r._headerPart;
		_body = r._body;
		_method = r._method;
		_path = r._path;
		_version = r._version;
		_query = r._query;
		_rawBuf = r._rawBuf;
		_headers = r._headers;
		_unchunked = r._unchunked;
		_printed = r._printed;
	}
	return *this;
}

// Methods

int	Request::create( const std::string & raw )
{
	_rawBuf.append(raw);

	size_t headerEnd = _rawBuf.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return 1;
	if (_headerPart.empty())
	{
		_headerPart = _rawBuf.substr(0, headerEnd);
		std::stringstream request(_headerPart);
		request >> _method >> _path >> _version;
		_version.erase(_version.find_last_not_of(" \t\r\n") + 1);

		size_t qpos = _path.find('?');
		if (qpos != std::string::npos)
		{
			_query = _path.substr(qpos + 1);
			_path = _path.substr(0, qpos);
		}

		std::string line = "";
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
	}
	size_t bodyStart = headerEnd + 4;
	if (_rawBuf.size() > bodyStart)
	{
		_body.insert(_body.end(), _rawBuf.begin() + bodyStart, _rawBuf.end());
		_rawBuf.erase(bodyStart);
	}
	if (!_isComplete())
		return 2;
	if (!getPrinted())
		_printed = true;
	return 0;
}

void	Request::reset( void )
{
	_headerPart.clear();
	_body.clear();
	_method.clear();
	_path.clear();
	_version.clear();
	_query.clear();
	_rawBuf.clear();
	_headers.clear();
	_unchunked = false;
	_printed = false; 
}

// Private methods

bool	Request::_isComplete( void )
{
	if (_headers.count("Content-Length"))
	{
		std::stringstream ss(_headers["Content-Length"]);
		size_t len = 0;
		ss >> len;
		return _body.size() >= len;
	}
	if (_headers.count("Transfer-Encoding"))
	{
		std::string te = _headers["Transfer-Encoding"];
		if (te.find("chunked") != std::string::npos)
		{
			std::string bodyStr(_body.begin(), _body.end());
			if (bodyStr.find("0\r\n\r\n") == std::string::npos)
				return false;
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

std::vector<char>	Request::_unchunkBody( const std::vector<char> & raw )
{
	std::vector<char> cleaned = raw;
	std::vector<char> result;
	const char * crlf = "\r\n";
	std::vector<char>::iterator firstCRLF = std::search(cleaned.begin(), cleaned.end(), crlf, crlf + 2);
	if (firstCRLF != cleaned.end())
	{
		std::string firstLine(cleaned.begin(), firstCRLF);
		char * endp = NULL;
		long globalLen = std::strtol(firstLine.c_str(), &endp, 16);
		if (endp != firstLine.c_str() && globalLen >= 0)
		{
			size_t posAfterHeader = (firstCRLF - cleaned.begin()) + 2;
			if (posAfterHeader + static_cast<size_t>(globalLen) <= cleaned.size())
				cleaned.erase(cleaned.begin(), cleaned.begin() + posAfterHeader);
		}
	}

	const char term[] = "0\r\n\r\n";
	if (cleaned.size() >= sizeof(term) - 1)
	{
		std::vector<char>::iterator it = std::search(cleaned.begin(), cleaned.end(), term, term + sizeof(term) - 1);
		if (it != cleaned.end() && it + (sizeof(term) - 1) == cleaned.end())
			cleaned.erase(it, cleaned.end());
	}

	size_t pos = 0;
	while (pos < cleaned.size())
	{
		size_t lineEnd = std::search(cleaned.begin() + pos, cleaned.end(), crlf, crlf + 2) - cleaned.begin();
		if (lineEnd >= cleaned.size())
			break;

		std::string sizeStr(cleaned.begin() + pos, cleaned.begin() + lineEnd);
		size_t semicolon = sizeStr.find(';');
		if (semicolon != std::string::npos)
			sizeStr = sizeStr.substr(0, semicolon);

		char * endPtr = NULL;
		long chunkSize = std::strtol(sizeStr.c_str(), &endPtr, 16);
		if (endPtr == sizeStr.c_str() || chunkSize < 0)
			break;
		pos = lineEnd + 2;
		if (chunkSize == 0)
			break;
		if (pos + static_cast<size_t>(chunkSize) > cleaned.size())
			break;
		result.insert(result.end(), cleaned.begin() + pos, cleaned.begin() + pos + chunkSize);
		pos += chunkSize;
		if (pos + 1 >= cleaned.size())
			break;
		if (cleaned[pos] != '\r' || cleaned[pos + 1] != '\n')
			break;
		pos += 2;
	}
	if (result.empty())
		return cleaned;
	return result;
}

// Getters

Client * Request::getClient() const { return _client; }
const std::string & Request::getHeaderPart() const { return _headerPart; }
const std::vector<char> & Request::getBody() const { return _body; }
const std::string & Request::getMethod() const { return _method; }
const std::string & Request::getPath() const { return _path; }
const std::string & Request::getVersion() const { return _version; }
const std::string & Request::getQuery() const { return _query; }
const std::map<std::string, std::string> & Request::getHeaders() const { return _headers; }
std::string Request::getHeader( const std::string & type ) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(type);
	if (it != _headers.end()) return it->second;
	return "";
}
bool Request::getUnchunked() const { return _unchunked; }
bool Request::getPrinted() const { return _printed; }
