/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:11:45 by layang            #+#    #+#             */
/*   Updated: 2025/09/02 18:19:30 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "main.hpp"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

HttpResponse::HttpResponse()
: _status(200), _statusText("OK") {}

HttpResponse::~HttpResponse() {}

/* void HttpResponse::executeCGI(const HttpRequest &req, const Location &loc)
{
	int inPipe[2];   // parent proc write to CGI
	int outPipe[2];  // CGI output to parent proc

	if (pipe(inPipe) < 0 || pipe(outPipe) < 0)
	{
		setStatus(500, "Internal Server Error");
		setBody("Failed to create pipes for CGI");
		return;
	}

	pid_t pid = fork();
	if (pid < 0)
	{
		setStatus(500, "Internal Server Error");
		setBody("Failed to fork CGI process");
		return;
	}
	if (pid == 0)
	{ // Child process
		close(inPipe[1]);
		close(outPipe[0]);

		dup2(inPipe[0], 0);   // CGI/uploa stdin
		dup2(outPipe[1], 1);  // CGI stdout
		close(inPipe[0]);
		close(outPipe[1]);

		// 构建 execve 参数
		const char *cgiPath = loc.getCgi().begin()->second.c_str(); // suppose only one CGI
		const char *argv[] = { cgiPath, req.getRequestBody().c_str(), NULL };
		char *envp[] = { NULL }; // add env vars，like CONTENT_LENGTH, REQUEST_METHOD

		execve(cgiPath, (char * const*)argv, envp);
		_exit(1); // execve failed
	}
	// Parent process
	close(inPipe[0]);
	close(outPipe[1]);

	// write request body to CGI stdin
	write(inPipe[1], req.getRequestBody().c_str(), req.getRequestBody().size());
	close(inPipe[1]);

	// read CGI stdout
	char buffer[4096];
	std::string cgiOutput;
	ssize_t n;
	while ((n = read(outPipe[0], buffer, sizeof(buffer))) > 0) {
		cgiOutput.append(buffer, n);
	}
	close(outPipe[0]);

	// wait for child proc to finish
	int status;
	waitpid(pid, &status, 0);

	// simply parse CGI output：headers and body
	size_t pos = cgiOutput.find("\r\n\r\n");
	if (pos != std::string::npos) {
		std::string rawHeaders = cgiOutput.substr(0, pos);
		_body = cgiOutput.substr(pos + 4);

		std::istringstream headerStream(rawHeaders);
		std::string line;
		while (std::getline(headerStream, line)) {
			size_t colon = line.find(':');
			if (colon != std::string::npos) {
				std::string key = line.substr(0, colon);
				std::string value = line.substr(colon + 1);
				if (!value.empty() && value[0] == ' ') value.erase(0,1);
				_headers[key] = value;
			}
		}
	} else {
		// CGI output without header
		_body = cgiOutput;
	}

	// set default status
	if (!_headers.count("Status"))
		setStatus(200, "OK");
	else {
		// Status like: "200 OK"
		std::istringstream ss(_headers["Status"]);
		int code;
		std::string text;
		ss >> code;
		std::getline(ss, text);
		setStatus(code, text);
	}
} */

// find location in server
const Location* HttpResponse::findLocation(const std::string &path, const Server* server) const
{
	const std::vector<Location>& locations = server->getLocations();
	const Location* best = NULL;

	for (size_t i = 0; i < locations.size(); ++i) {
		if (path.find(locations[i].getPath()) == 0) {
			if (!best || locations[i].getPath().size() > best->getPath().size())
				best = &locations[i];
		}
	}
	return best;
}

bool HttpResponse::allowsMethod(const Location* loc, const std::string &method) const
{
	const std::vector<std::string> &methods = loc->getMethods();
	for (size_t i = 0; i < methods.size(); ++i) {
		if (methods[i] == method)
			return true;
	}
	return false;
}

void HttpResponse::setStatus(int status, const std::string &text)
{
	_status = status;
	_statusText = text;
}

void HttpResponse::setBody(const std::string &body)
{
	_body = body;
}

void HttpResponse::setHeader(const std::string &key, const std::string &value)
{
	_headers[key] = value;
}

std::string HttpResponse::buildHeaders() const
{
	std::ostringstream ss;
	ss << "HTTP/1.1 " << _status << " " << _statusText << "\r\n";
	for (std::map<std::string,std::string>::const_iterator it=_headers.begin(); it!=_headers.end(); ++it) {
		ss << it->first << ": " << it->second << "\r\n";
	}
	ss << "\r\n"; // header-body sep
	return ss.str();
}

/* std::string HttpResponse::toString() const
{
	return buildHeaders() + _body;
} */

std::string HttpResponse::toString() const
{
    std::ostringstream response;

    // Status line
    response << "HTTP/1.1 " << _status << " " << _statusText << "\r\n";

    // Required headers
    if (_headers.find("Content-Type") == _headers.end())
        response << "Content-Type: text/html; charset=UTF-8\r\n";

    // Content-Length header
    std::ostringstream oss;
    oss << _body.size();
    response << "Content-Length: " << oss.str() << "\r\n";

    // Other custom headers
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
         it != _headers.end(); ++it)
    {
        response << it->first << ": " << it->second << "\r\n";
    }

    // Empty line separates headers and body
    response << "\r\n";

    // Body
    response << _body;

    return response.str();
}


// server 

// location /bee
// 	root /upload

// // location : /e
// http::localhos:80/bee


// site/
// 	index.html
// 	upload/
// 		coucou.html


// treat POST request
void HttpResponse::handlePost(const HttpRequest &req, const Location *loc)
{
	std::string path = req.getPath();
	std::string contentType = req.getHeader("Content-Type");

	// upload file
	if (!loc->getUpload().empty() && contentType.find("multipart/form-data") != std::string::npos)
	{
		bool success = saveUploadedFile(req, loc->getUpload());
		if (success) {
			setStatus(201, "Created");
			setBody("File uploaded successfully");
		} else {
			setStatus(500, "Internal Server Error");
			setBody("Failed to save uploaded file");
		}
		return;
	}

	// register
	if (path == "/register" && contentType == "application/x-www-form-urlencoded")
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

	// login
	if (path == "/login" && contentType == "application/x-www-form-urlencoded")
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
}

void HttpResponse::buildResponse(HttpRequest &req, const Server* server)
{
    // 1. Find the matching location for the requested path
    const Location* loc = findLocation(req.getPath(), server);
    if (!loc) {
        // No matching location → return 404
        setStatus(404, "Not Found");

        // Try to use the server's custom 404 page if defined
        const std::map<int, std::string> &errPages = server->getErrorPages();
        if (errPages.find(404) != errPages.end())
            setBody(readFile(errPages.at(404)));
        else
            setBody("No matching location");

        return;
    }

    // 2. Check if the HTTP method is allowed
    if (!allowsMethod(loc, req.getMethod())) {
        setStatus(405, "Method Not Allowed");
        const std::vector<std::string> &methods = loc->getMethods();
        std::string allowHeader;
        for (size_t i = 0; i < methods.size(); ++i) {
            allowHeader += methods[i];
            if (i != methods.size() - 1)
                allowHeader += ", ";
        }
        setHeader("Allow", allowHeader);
        return;
    }

    // 3. Check for redirection
	// The client (browser) will automatically make another request based on the Location header.
    const std::map<int, std::string> &redir = loc->getReturn();
    if (!redir.empty()) {
        std::string locationUrl = redir.begin()->second;
        setStatus(302, "Found");
        setHeader("Location", locationUrl);
        return;
    }

    // 4. Handle POST requests
    if (req.getMethod() == "POST") {
		//For example, handle uploads or size limits
		if (req.getRequestBody().size() > server->getMaxSize()) {
		    setStatus(413, "Payload Too Large");
			req.discardBody();
		    return;
		}
        handlePost(req, loc);
        return;
    }

    // 5. Build the full file path to serve
    std::string filePath = resolvePath(loc, req.getPath());
	std::cout << "Checking loc: " << loc->getPath() << std::endl;
    // 6. Check if the path exists
    int status = acstat(filePath.c_str(), F_OK | R_OK);
	std::cout << "Checking path: " << filePath << ", status = " << status << std::endl;
    if (status == 1) { // Regular file
        setStatus(200, "OK");
        setBody(readFile(filePath));
        return;
    } 
    else if (status == 2) { // Directory
        // try index files
		std::cout << "filePath in Directory: " << filePath << std::endl;
		if (filePath[filePath.size() - 1] != '/')
        	filePath += '/';  // ensure ending slash before adding index
		const std::vector<std::string> &indexList = loc->getIndex();
		for (std::vector<std::string>::const_iterator it = indexList.begin();
			it != indexList.end(); ++it)
		{
			std::cout << "Passing to index file: " << *it << std::endl;
			std::string indexPath = *it;
			// if (acstat(indexPath.c_str(), F_OK | R_OK) == 1) {
			// 	setStatus(200, "OK");
			// 	setBody(readFile(indexPath));
			// 	return;  // index file found
			// }
			
			std::cout << "Trying index file: " << indexPath << std::endl;
			int s = acstat(indexPath.c_str(), F_OK | R_OK);
			std::cout << "acstat returned: " << s << std::endl;

			if (s == 1) {
				setStatus(200, "OK");
				setBody(readFile(indexPath));
				return;
    		}
		}

		// autoindex
		if (loc->getAutoindex()) {
			setStatus(200, "OK");
			setBody(generateDirectoryListing(filePath));
			return;
		}
        // Directory exists but no index file → return 404
        setStatus(404, "Not Found");
        const std::map<int, std::string> &errPages = server->getErrorPages();
        if (errPages.find(404) != errPages.end())
            setBody(readFile(errPages.find(404)->second));
        else
            setBody("Directory exists but no index file, File not found");
        return;
    }

    // Path does not exist → return 404
    setStatus(404, "Not Found");
    const std::map<int, std::string> &errPages = server->getErrorPages();
    if (errPages.find(404) != errPages.end())
        setBody(readFile(errPages.find(404)->second));
    else
        setBody("Path does not exist, File not found");
}
