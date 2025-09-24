/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:11:45 by layang            #+#    #+#             */
/*   Updated: 2025/09/24 13:39:05 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

HttpResponse::HttpResponse()
: _status(200), _statusText("OK") {}

HttpResponse::~HttpResponse() {}

void HttpResponse::executeCGI(const HttpRequest &req,
                              const Location &loc,
                              const std::string &filePath,
                              const Server &server)
{
    int inPipe[2];
    int outPipe[2];

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
    {
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

        const std::map<std::string, std::string> &cgiMap = loc.getCgi();
        std::map<std::string, std::string>::const_iterator it = cgiMap.find(ext);
        if (it == cgiMap.end()) {
            _exit(1);
        }
        const char *cgiPath = it->second.c_str(); 
        const char *scriptPath = filePath.c_str();
        const char *argv[] = { cgiPath, scriptPath, NULL };

        std::vector<std::string> env = buildCgiEnv(req, filePath, server);
		
		std::vector<char*> envp;
		std::vector<char*> buffers;
		for (size_t i = 0; i < env.size(); ++i) {
			char* copy = new char[env[i].size() + 1]; 
			std::strcpy(copy, env[i].c_str());
			envp.push_back(copy);
			buffers.push_back(copy);
		}
		envp.push_back(NULL);
        execve(cgiPath, (char * const*)argv, (char * const*)&envp[0]);
		for (size_t i = 0; i < buffers.size(); ++i)
   			delete[] buffers[i];
        _exit(1);
    }
    close(inPipe[0]); 
    close(outPipe[1]);
    if (req.getMethod() == "POST")
    {
        write(inPipe[1], req.getRequestBody().c_str(), req.getRequestBody().size());
    }
    close(inPipe[1]);

    char buffer[4096];
    std::string cgiOutput;
    ssize_t n;
    while ((n = read(outPipe[0], buffer, sizeof(buffer))) > 0) {
        cgiOutput.append(buffer, n);
    }
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

	if (headerEndPos != std::string::npos) {
		rawHeaders = cgiOutput.substr(0, headerEndPos);
		size_t bodyStart = (headerEndPos == rnPos) ? headerEndPos + 4 : headerEndPos + 2;
		body = cgiOutput.substr(bodyStart);
		std::istringstream headerStream(rawHeaders);
		std::string line;
		while (std::getline(headerStream, line)) {
			if (!line.empty() && line[line.size() - 1] == '\r') {
				line.erase(line.size() - 1, 1);
			}
			size_t colon = line.find(':');
			if (colon != std::string::npos) {
				std::string key = line.substr(0, colon);
				std::string value = line.substr(colon + 1);
				if (!value.empty() && value[0] == ' ')
					value.erase(0, 1);
				_headers[key] = value;
			}
		}
	} else {
		body = cgiOutput;
	}
    if (!_headers.count("Content-Type"))
        _headers["Content-Type"] = "text/html";

    for (std::map<std::string, std::string>::iterator it = _headers.begin();
         it != _headers.end(); ++it)
    {
        setHeader(it->first, it->second);
    }
    setBody(body);
    if (!_headers.count("Status"))
        setStatus(200, "OK");
    else {
        std::istringstream ss(_headers["Status"]);
        int code;
        std::string text;
        ss >> code;
        std::getline(ss, text);
        setStatus(code, text);
    }
}

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
	ss << "\r\n";
	return ss.str();
}

std::string HttpResponse::toString(const HttpRequest &req) const
{
    std::ostringstream response;

    response << "HTTP/1.1 " << _status << " " << _statusText << "\r\n";
    if (_headers.find("Content-Type") == _headers.end())
	{
        std::string type = getContentType(req.getPath());
		response << "Content-Type: " << type << "; charset=UTF-8\r\n";		
	}
    std::ostringstream oss;
    oss << _body.size();
    response << "Content-Length: " << oss.str() << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
         it != _headers.end(); ++it)
    {
        response << it->first << ": " << it->second << "\r\n";
    }
    response << "\r\n";
    response << _body;
    return response.str();
}

void HttpResponse::handlePost(const HttpRequest &req, const Location *loc)
{
	std::string path = req.getPath();
	std::string contentType = req.getHeader("Content-Type");
	if (path == "/upload" && !loc->getUpload().empty())
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
}

const Location* HttpResponse::pathPrepa(HttpRequest &req, const Server* server)
{
	const Location* loc = findLocation(req.getPath(), server);
    if (!loc) {
        setStatus(404, "Not Found");
        setBody("No matching location");
        return NULL;
    }
	
	const std::map<int, std::string> &redir = loc->getReturn();
	if (!redir.empty()) {
		int code = redir.begin()->first;
		std::string locationUrl = redir.begin()->second;
		setStatus(code, (code == 301 ? "Moved Permanently" : "Found"));
		setHeader("Location", locationUrl);
		return NULL;
	}

	if (!allowsMethod(loc, req.getMethod())) {
        setStatus(405, "Method Not Allowed");
		setBody("Method Not Allowed");
        const std::vector<std::string> &methods = loc->getMethods();
        std::string allowHeader;
        for (size_t i = 0; i < methods.size(); ++i) {
            allowHeader += methods[i];
            if (i != methods.size() - 1)
                allowHeader += ", ";
        }
        setHeader("Allow", allowHeader);
        return NULL;
    }
	
	return loc;
}

void HttpResponse::returnResponse(const Location* loc, HttpRequest &req, const Server* server, SessionManager& sessionManager)
{
	//(void)sessionManager;
	if (req.getMethod() == "DELETE") {

		if (loc->getPath() != "/upload") {
			setStatus(403, "Forbidden");
			setHeader("Content-Type", "application/json");
			setBody("{\"status\":\"error\",\"message\":\"DELETE allowed only in /upload\"}");

			return;
		}

		std::string filePath = resolvePath(loc, req.getPath());
		int status = acstat_file(filePath.c_str(), F_OK | R_OK);
		if (status == 1) {
			// Regular file → try to delete
			if (std::remove(filePath.c_str()) == 0) {
				setStatus(200, "OK");
				setHeader("Content-Type", "application/json");
				setBody("{\"status\":\"deleted\",\"path\":\"" + req.getPath() + "\"}");
			} else {
				setStatus(500, "Internal Server Error");
				setHeader("Content-Type", "application/json");
				setBody("{\"status\":\"error\",\"message\":\"Failed to delete file\"}");
			}
			return;
		} else if (status == 2) {
			setStatus(405, "Method Not Allowed");
			setHeader("Content-Type", "application/json");
			setBody("{\"status\":\"error\",\"message\":\"Cannot delete directory\"}");
			return;
		} else {
			setStatus(404, "Not Found");
			setHeader("Content-Type", "application/json");
			setBody("{\"status\":\"error\",\"message\":\"File not found\"}");
			return;
		}
	}

    if (req.getMethod() == "POST") {
		if (req.getRequestBody().size() > server->getMaxSize()) {
		    setStatus(413, "Payload Too Large");
			setBody("Failed to save uploaded file: > 1M");
			req.discardBody();
		    return;
		}
        handlePost(req, loc);
        return;
    }
    std::string filePath = resolvePath(loc, req.getPath());
    int status = acstat_file(filePath.c_str(), F_OK | R_OK);
    if (status == 1) {
		std::string ext;
		size_t dotPos = filePath.rfind('.');
		if (dotPos != std::string::npos)
			ext = filePath.substr(dotPos);
		const std::map<std::string, std::string> &cgiMap = loc->getCgi();
		std::map<std::string, std::string>::const_iterator it = cgiMap.find(ext);
		if (it != cgiMap.end()) {
			executeCGI(req, *loc, filePath, *server);
			return;
		}
        if (req.getMethod() == "GET") {
			std::string html = readFile(filePath);
			if (filePath.find("index.html") != std::string::npos) {
				std::string cookieHeader = req.getHeader("Cookie");
				Session* session = sessionManager.getSession(getSessionIdFromCookie(cookieHeader));
				if (!session) {
					std::string sid = sessionManager.createSession("guest");
					std::string value = "session_id=" + sid + "; Max-Age=3600; HttpOnly";
					setHeader("Set-Cookie", value);
					session = sessionManager.getSession(sid);
				}
				if (session) {
					std::string mode = session->mode;
					size_t pos = html.find("{{MODE}}");
					if (pos != std::string::npos)
						html.replace(pos, 8, mode);
				}
			}
			setStatus(200, "OK");
			setBody(html);
			setHeader("Content-Type", getContentType(filePath));
			return;
			// setStatus(200, "OK");
			// setBody(readFile(filePath));
			// return;
        }

        setStatus(405, "Method Not Allowed");
        setBody("POST not allowed on static file");
        return;
    } 
    else if (status == 2) {
		std::cout << "filePath in Directory: " << filePath << std::endl;
		if (filePath[filePath.size() - 1] != '/')
        	filePath += '/';
		const std::vector<std::string> &indexList = loc->getIndex();
		std::string root = loc->getRoot();
		for (std::vector<std::string>::const_iterator it = indexList.begin();
			it != indexList.end(); ++it)
		{
			std::string indexPath = combineIndexPath(filePath, *it, root);
			int s = acstat_file(indexPath.c_str(), F_OK | R_OK);
			if (s == 1) {
				std::string html = readFile(indexPath);
				std::string cookieHeader = req.getHeader("Cookie");
				Session* session = sessionManager.getSession(getSessionIdFromCookie(cookieHeader));
				if (!session) {
					std::string sid = sessionManager.createSession("guest");
					std::string value = "session_id=" + sid + "; Max-Age=3600; HttpOnly";
					setHeader("Set-Cookie", value);
					session = sessionManager.getSession(sid);
				}
				if (session) {
					std::string mode = session->mode;
					size_t pos = html.find("{{MODE}}");
					if (pos != std::string::npos)
						html.replace(pos, 8, mode);
				}			
				setStatus(200, "OK");
				setBody(html);
				setHeader("Content-Type", getContentType(indexPath));
				return;				
				// setStatus(200, "OK");
				// setBody(readFile(indexPath));
				// setHeader("Content-Type", getContentType(indexPath));
				// return;
    		}
		}		

		if (loc->getAutoindex()) {
			setHeader("Content-Type", "text/html");
			setStatus(200, "OK");
			setBody(generateDirectoryListing(filePath, req.getPath()));
			return;
		}

        setStatus(404, "Not Found");
        const std::map<int, std::string> &errPages = server->getErrorPages();
        if (errPages.find(404) != errPages.end())
		{
            setBody(readFile(errPages.find(404)->second));
			setHeader("Content-Type", "text/html");
		}
        else
            setBody("Directory exists but no index file, File not found");
        return;
    }

    setStatus(404, "Not Found");
    const std::map<int, std::string> &errPages = server->getErrorPages();
    if (errPages.find(404) != errPages.end())
	{
        setBody(readFile(errPages.find(404)->second));
		setHeader("Content-Type", "text/html");
	}
    else
        setBody("Path does not exist, File not found");
}

void HttpResponse::buildResponse(HttpRequest &req, const Server* server, SessionManager& sessionManager)
{
	const Location* loc = pathPrepa(req, server);
	if (loc)
		returnResponse(loc, req, server, sessionManager);
}
