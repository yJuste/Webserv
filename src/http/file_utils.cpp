/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_utils.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/31 17:42:45 by layang            #+#    #+#             */
/*   Updated: 2025/09/24 12:16:08 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"  

std::string getCurrentWorkingDirectory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        return std::string(cwd);
    } else {
        perror("getcwd() error");
        return std::string("");
    }
}

long getFileSize(const std::string &path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
        return -1;
    return static_cast<long>(info.st_size);
}

std::string combineIndexPath(const std::string &dirPath, const std::string &index, const std::string &root)
{
    std::string cleanIndex = index;
    if (root.size() > 0 && cleanIndex.find(root) == 0)
        cleanIndex = cleanIndex.substr(root.size());
    std::string finalPath = dirPath;
    if (finalPath.empty() || finalPath[finalPath.size() - 1] != '/')
        finalPath += '/';
    finalPath += cleanIndex;

    return finalPath;
}

std::string generateDirectoryListing(const std::string &dirPath, const std::string &reqPath)
{
    DIR *dir;
    struct dirent *entry;
    std::ostringstream html;

    dir = opendir(dirPath.c_str());
    if (!dir)
        return "<html><body><h1>403 Forbidden</h1></body></html>";

    html << "<html><head><title>Index of " << reqPath
         << "</title></head><body>\n";
    html << "<h1>Index of " << reqPath << "</h1><ul>\n";

    while ((entry = readdir(dir)) != NULL)
    {
        std::string name(entry->d_name);
        if (name == ".")
            continue;

        std::string href = reqPath;
        if (href[href.size() - 1] != '/')
            href += '/';
        href += name;

        html << "<li><a href=\"" << href << "\">" << name << "</a></li>\n";
    }
    closedir(dir);

    html << "</ul></body></html>\n";
    return html.str();
}


bool userExists(const std::string &username)
{
	std::ifstream file("users.db");
	std::string line;
	while (std::getline(file, line)) {
		size_t sep = line.find(':');
		if (sep != std::string::npos) {
			std::string u = line.substr(0, sep);
			if (u == username)
				return true;
		}
	}
	return false;
}

std::string getFormValue(const std::string &body, const std::string &key)
{
    size_t pos = body.find(key + "=");
    if (pos == std::string::npos)
        return "";

    pos += key.size() + 1;
    size_t end = body.find('&', pos);
    if (end == std::string::npos)
        end = body.size();

    return body.substr(pos, end - pos);
}

bool saveUser(const std::string &username, const std::string &password)
{
	if (userExists(username))
		return false;
	std::ofstream file("users.db", std::ios::app);
	if (!file)
		return false;
	file << username << ":" << password << "\n";
	file.close();
	return true;
}

bool checkUser(const std::string &username, const std::string &password)
{
	std::ifstream file("users.db");
	std::string line;
	while (std::getline(file, line)) {
		size_t sep = line.find(':');
		if (sep != std::string::npos) {
			std::string u = line.substr(0, sep);
			std::string p = line.substr(sep + 1);
			if (u == username && p == password)
				return true;
		}
	}
	return false;
}

bool saveUploadedFile(const HttpRequest &req, const std::string &uploadDir)
{
    std::string body = req.getRequestBody();
    std::string contentType = req.getHeader("Content-Type");
    size_t pos = contentType.find("boundary=");
    if (pos == std::string::npos)
        return false;
    std::string boundary = "--" + contentType.substr(pos + 9);
    size_t start = body.find(boundary);
    if (start == std::string::npos)
        return false;
    size_t fnStart = body.find("filename=\"", start);
    if (fnStart == std::string::npos)
        return false;

    fnStart += 10;
    size_t fnEnd = body.find("\"", fnStart);
    std::string filename = body.substr(fnStart, fnEnd - fnStart);
    size_t contentStart = body.find("\r\n\r\n", fnEnd);
    if (contentStart == std::string::npos)
        return false;
    contentStart += 4;
    size_t contentEnd = body.find(boundary, contentStart) - 2;
    if (contentEnd <= contentStart)
        return false;
    std::string fileContent = body.substr(contentStart, contentEnd - contentStart);
	std::ofstream out((uploadDir + "/" + filename).c_str(), std::ios::binary);
    if (!out)
        return false;
    out.write(fileContent.c_str(), fileContent.size());
    out.close();
    return true;
}

std::string resolvePath(const Location* loc, const std::string &reqPath)
{
    std::string root = loc->getRoot();
    if (root[0] == '.' && root[1] == '/')
        root = getCurrentWorkingDirectory() + root.erase(root.size() - 2) + loc->getPath();
    else
        root = getCurrentWorkingDirectory() + "/" + root;
    while (!root.empty() && root[root.size() - 1] == '/')
        root.erase(root.size() - 1);
    std::string locPath = loc->getPath();
    std::string path = reqPath;
    if (!locPath.empty() && locPath != "/" && path.find(locPath) == 0)
        path = path.substr(locPath.size());
    while (!path.empty() && path[0] == '/')
        path.erase(0, 1);

    if (!path.empty())
        return root + "/" + path;
    else     
        return root;
}

std::string readFile(const std::string &path)
{
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    if (!file)
        return "";
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string getContentType(const std::string &path)
{
	if (path.length() >= 5 && path.substr(path.length() - 5) == ".html")
		return "text/html; charset=UTF-8";
	if (path.length() >= 4 && path.substr(path.length() - 4) == ".htm")
		return "text/html; charset=UTF-8";
	if (path.length() >= 4 && path.substr(path.length() - 4) == ".css")
		return "text/css; charset=UTF-8";
	if (path.length() >= 3 && path.substr(path.length() - 3) == ".js")
		return "application/javascript; charset=UTF-8";
	if (path.length() >= 4 && path.substr(path.length() - 4) == ".jpg")
		return "image/jpeg";
	if (path.length() >= 5 && path.substr(path.length() - 5) == ".jpeg")
		return "image/jpeg";
	if (path.length() >= 4 && path.substr(path.length() - 4) == ".png")
		return "image/png";
	if (path.length() >= 4 && path.substr(path.length() - 4) == ".gif")
		return "image/gif";
	return "text/plain; charset=UTF-8";
}

std::string portsToString(const std::vector<int> &ports) {
    std::ostringstream oss;
    for (size_t i = 0; i < ports.size(); ++i) {
        if (i != 0) oss << ",";
        oss << ports[i];
    }
    return oss.str();
}

std::vector<std::string> buildCgiEnv(const HttpRequest &req,
                                            const std::string &filePath,
                                            const Server &server)
{
    std::vector<std::string> env;

    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    env.push_back("REQUEST_METHOD=" + req.getMethod());
    env.push_back("SCRIPT_FILENAME=" + filePath);
    env.push_back("SCRIPT_NAME=" + req.getPath());
    env.push_back("QUERY_STRING=" + req.getQueryString());

    env.push_back("CONTENT_LENGTH=" + pushToString(req.getRequestBody().size())); 
    env.push_back("CONTENT_TYPE=" + req.getHeader("Content-Type"));

    env.push_back("SERVER_NAME=" + req.getHeader("Host"));
    env.push_back("SERVER_PORT=" + portsToString(server.getPort()));

    return env;
}

int	acstat_file( const char * path, int mode )
{
	struct stat	buf;

	if (stat(path, &buf) == -1)
		return -1;
	if (access(path, mode) == -1)
		return -1;
	if (S_ISREG(buf.st_mode))
		return 1;
	if (S_ISDIR(buf.st_mode))
	{
		DIR *dir = opendir(path);
		if (!dir)
			return -1;
		closedir(dir);
		return 2;
	}
	return -1;
}

std::string getSessionIdFromCookie(const std::string &cookie)
{
    size_t pos = cookie.find("session_id=");
    if (pos == std::string::npos)
        return "";
    pos += 11;
    size_t end = cookie.find(';', pos);
    if (end == std::string::npos)
        return cookie.substr(pos);
    else
        return cookie.substr(pos, end - pos);
}
