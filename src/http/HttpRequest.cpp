/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 19:13:27 by layang            #+#    #+#             */
/*   Updated: 2025/09/02 17:31:47 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include <string>
#include <map>
#include <sstream>
#include <cstdlib>

HttpRequest::HttpRequest()
	: _method(""), _path(""), _httpVersion(""), _body(""),
	  _unchunked("false")
{
}

HttpRequest::~HttpRequest()
{
}

// member functions
/* void HttpRequest::parseRequest(const std::string &rawRequest)
{
	//std::cout << "Request received: " << std::endl;
	//std::cout << rawRequest << std::endl;
	
	// 1. parse method, path, httpVersion
	std::istringstream request(rawRequest);
	request >> _method >> _path >> _httpVersion;
	
    // 2. split and parse headers
	std::string line;
	while (std::getline(request, line) && line != "\r")
	{
		if (line.empty() || line == "\r")
			break;
		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos)
		{
			std::string key = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 1);
			key.erase(key.find_last_not_of(" \t\r\n") + 1);
			value.erase(0, value.find_first_not_of(" \t\r\n"));
			value.erase(value.find_last_not_of(" \t\r\n") + 1);
			_headers[key] = value;
		}
	}
	// 3. save _body（probably not finished）
	std::string remaining;
	std::getline(request, remaining, '\0');
	_body += remaining;
} */

void HttpRequest::parseRequest(const std::string &rawRequest)
{
    // 1. Parse request line: method, path, HTTP version
    std::istringstream request(rawRequest);
    request >> _method >> _path >> _httpVersion;

    // Remove any trailing '\r' from _path and _httpVersion
    if (!_path.empty() && _path[_path.size() - 1] == '\r')
        _path = _path.substr(0, _path.size() - 1);
    if (!_httpVersion.empty() && _httpVersion[_httpVersion.size() - 1] == '\r')
        _httpVersion = _httpVersion.substr(0, _httpVersion.size() - 1);

    // 2. Parse headers
    std::string line;
    while (std::getline(request, line) && line != "\r")
    {
        if (line.empty() || line == "\r")
            break;

        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos)
        {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);

            // Trim whitespace from key and value
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            _headers[key] = value;
        }
    }

    // 3. Read the body (if any)
    std::string remaining;
    std::getline(request, remaining, '\0'); // read until EOF
    _body += remaining;
}

void HttpRequest::discardBody()
{
	_body.clear();
}

void HttpRequest::reset()
{
    _method.clear();
    _path.clear();
    _httpVersion.clear();
    _headers.clear();
    _body.clear();
	_unchunked = false;
}

std::string HttpRequest::getMethod() const
{
	return this->_method;
}

std::string HttpRequest::getPath() const
{
	return this->_path;
}

std::string HttpRequest::getHttpVersion() const
{
	return this->_httpVersion;
}
std::string HttpRequest::getRequestBody() const
{
	return this->_body;
}

std::string HttpRequest::getHeader(const std::string &key) const
{
    std::map<std::string, std::string>::const_iterator it = _headers.find(key);
    if (it != _headers.end())
        return it->second;
    return "";
}

bool HttpRequest::isComplete()
{
	// case 1: has Content-Length
	if (_headers.count("Content-Length"))
	{
		int len = std::atoi(_headers["Content-Length"].c_str());
		return (_body.size() >= (size_t)len);
	}

	// case 2: chunked
	if (_headers.count("Transfer-Encoding") && _headers["Transfer-Encoding"] == "chunked")
	{
		if (_body.find("0\r\n\r\n") != std::string::npos)
		{
			if (!_unchunked)
			{
				_body = unchunkBody(_body);
				_unchunked = true;
			}
			return true;
		}
		return false;
	}

	// case 3: GET/HEAD without body
	return true;
}

std::string HttpRequest::unchunkBody(const std::string &raw)
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
