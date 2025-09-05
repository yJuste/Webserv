/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_utils.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/31 17:42:45 by layang            #+#    #+#             */
/*   Updated: 2025/09/04 11:15:40 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"          // acstat, READ_SIZE, project macros
//#include "HttpRequest.hpp"
//#include "Location.hpp"

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


std::string generateDirectoryListing(const std::string &path)
{
	DIR *dir;
	struct dirent *entry;
	std::ostringstream html;

	dir = opendir(path.c_str());
	if (!dir)
		return "<html><body><h1>403 Forbidden</h1></body></html>";

	html << "<html><head><title>Index of " << path
			<< "</title></head><body>\n";
	html << "<h1>Index of " << path << "</h1><ul>\n";

	while ((entry = readdir(dir)) != NULL)
	{
		std::string name(entry->d_name);
		if (name == ".")
			continue;
		html << "<li><a href=\"" << name << "\">"
				<< name << "</a></li>\n";
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

// create new user
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

// check login successful
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

// save upload
bool saveUploadedFile(const HttpRequest &req, const std::string &uploadDir)
{
    std::string body = req.getRequestBody();
    std::string contentType = req.getHeader("Content-Type");

    // only treat multipart/form-data
    size_t pos = contentType.find("boundary=");
    if (pos == std::string::npos)
        return false;

    std::string boundary = "--" + contentType.substr(pos + 9); // boundary= 
    size_t start = body.find(boundary);
    if (start == std::string::npos)
        return false;

    // find file name
    size_t fnStart = body.find("filename=\"", start);
    if (fnStart == std::string::npos)
        return false;

    fnStart += 10; // jump filename="
    size_t fnEnd = body.find("\"", fnStart);
    std::string filename = body.substr(fnStart, fnEnd - fnStart);

    // find file content
    size_t contentStart = body.find("\r\n\r\n", fnEnd);
    if (contentStart == std::string::npos)
        return false;

    contentStart += 4; // jump \r\n\r\n
    size_t contentEnd = body.find(boundary, contentStart) - 2; // remove last \r\n

    if (contentEnd <= contentStart)
        return false;

    std::string fileContent = body.substr(contentStart, contentEnd - contentStart);

    // save file
    //std::ofstream out(uploadDir + "/" + filename, std::ios::binary);
	std::ofstream out((uploadDir + "/" + filename).c_str(), std::ios::binary);
    if (!out)
        return false;

    out.write(fileContent.c_str(), fileContent.size());
    out.close();

    return true;
}

/* std::string resolvePath(const Location* loc, const std::string &reqPath)
{
    std::string root = loc->getRoot();
    if (!root.empty() && root[0] != '/')
        root = getCurrentWorkingDirectory() + "/" + root;
    if (!root.empty() && root[root.size() - 1] != '/')
        root += '/';

    std::string path = reqPath;
    std::string locPath = loc->getPath();

    // remove location path prefix
    if (locPath != "/" && path.find(locPath) == 0)
        path = path.substr(locPath.size());

    // remove  /
    if (!path.empty() && path[0] == '/')
        path = path.substr(1);

    return root + path;
} */

/* std::string resolvePath(const Location* loc, const std::string &reqPath)
{
    std::string root = loc->getRoot();
    if (!root.empty() && root[0] != '/')
        root = getCurrentWorkingDirectory() + "/" + root;
    if (!root.empty() && root[root.size() - 1] != '/')
        root += '/';

    std::string path = reqPath;
    std::string locPath = loc->getPath();

    // remove location path prefix
    if (locPath != "/" && path.find(locPath) == 0)
        path = path.substr(locPath.size());

    // remove  /
    if (!path.empty() && path[0] == '/')
        path = path.substr(1);

    return root + path;
} */

std::string resolvePath(const Location* loc, const std::string &reqPath)
{
    std::string root = loc->getRoot();

    if (root.empty())
    {
        root = getCurrentWorkingDirectory(); 
        if (root[root.size() - 1] != '/')
            root += '/';
        std::cout << "Initial root: " << root << std::endl;
    }
    else
    {
        if (root[0] != '/')
            root = getCurrentWorkingDirectory() + "/" + root;
        std::cout << "Root after making absolute if needed: " << root << std::endl;

         if (root[root.size() - 1] != '/')
            root += '/';
        std::cout << "Root after ensuring trailing '/': " << root << std::endl;
    }
    

    std::string path = reqPath;
    std::cout << "Initial request path: " << path << std::endl;

    std::string locPath = loc->getPath();
    std::cout << "Location path: " << locPath << std::endl;

    // remove location path prefix
    if (locPath != "/" && path.find(locPath) == 0)
        path = path.substr(locPath.size());
    std::cout << "Path after removing location prefix: " << path << std::endl;

    // remove leading '/'
    if (!path.empty() && path[0] == '/')
        path = path.substr(1);
    std::cout << "Path after removing leading '/': " << path << std::endl;

    std::string finalPath = root + path;
    std::cout << "Final resolved path: " << finalPath << std::endl;
    std::cout << "" << std::endl;
    return finalPath;
}


/* std::string readFile(const std::string &path)
{
	std::ifstream file(path.c_str());
	if (!file)
		return "";
	std::ostringstream oss;
	std::string line;
	while (std::getline(file, line))
		oss << line << "\n";
	file.close();
	return oss.str();
} */

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
