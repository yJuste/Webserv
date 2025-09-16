/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 19:13:27 by layang            #+#    #+#             */
/*   Updated: 2025/09/16 19:02:15 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
	: _method(""), _path(""), _query_str(""), _httpVersion(""), _body(""), _headerPart(""),
	  _unchunked(false), _printed(false)
{
}

HttpRequest::~HttpRequest()
{
}

void HttpRequest::parseRequest(const std::string &rawRequest)
{
    size_t headerEnd = rawRequest.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return;

    _headerPart = rawRequest.substr(0, headerEnd);
    _body = rawRequest.substr(headerEnd + 4);
    std::istringstream request(_headerPart);
    request >> _method >> _path >> _httpVersion;
    if (!_path.empty() && _path[_path.size() - 1] == '\r')
        _path = _path.substr(0, _path.size() - 1);

    if (!_httpVersion.empty() && _httpVersion[_httpVersion.size() - 1] == '\r')
        _httpVersion = _httpVersion.substr(0, _httpVersion.size() - 1);
    size_t qpos = _path.find('?');
    if (qpos != std::string::npos) {
        _query_str = _path.substr(qpos + 1);
        _path = _path.substr(0, qpos);
    } else {
        _query_str.clear();
    }

    std::string line;
    std::getline(request, line);
    while (std::getline(request, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line = line.substr(0, line.size() - 1);

        if (line.empty())
            break;

        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos)
        {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            _headers[key] = value;
        }
    }
}

void HttpRequest::discardBody()
{
	_body.clear();
}

void HttpRequest::reset()
{
    _method.clear();
    _path.clear();
    _query_str.clear();
    _httpVersion.clear();
    _headers.clear();
    _body.clear();
    _headerPart.clear();
    _unchunked = false;
    _printed = false; 
}

std::string HttpRequest::getMethod() const
{
	return this->_method;
}

std::string HttpRequest::getPath() const
{
	return this->_path;
}

std::string HttpRequest::getQueryString() const
{
    return _query_str;
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

std::map<std::string, std::string> HttpRequest::getHeaders() const
{
    return _headers;
}

bool HttpRequest::hasPrinted() const { return _printed; }
void HttpRequest::setPrinted(bool val) { _printed = val; }

bool HttpRequest::isComplete()
{

    if (_headers.count("Content-Length"))
    {
        int len = std::atoi(_headers["Content-Length"].c_str());
        bool complete = (_body.size() >= (size_t)len);
        return complete;
    }

    if (_headers.count("Transfer-Encoding") && _headers["Transfer-Encoding"] == "chunked")
    {
        size_t pos = _body.find("0\r\n\r\n");
        if (pos != std::string::npos)
        {;
            if (!_unchunked)
            {
                _body = unchunkBody(_body);                
                _unchunked = true;
            }
            return true;
        }
        return false;
    }
    return true;
}

std::string HttpRequest::unchunkBody(const std::string &raw)
{
    std::string result;
    size_t pos = 0;
    int chunkIndex = 0;

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
        chunkIndex++;
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


