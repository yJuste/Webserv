// ************************************************************************** //
//                                                                            //
//                Response.cpp                                                //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Layang                                        //
//                                                                            //
// ************************************************************************** //

# include "Response.hpp"

Response::Response() : _status(200, "OK"), _body("") {}
Response::~Response() {}

// Method

void	Response::build( Request & req, const Server * server )
{
	const Location * loc = _findLocation(req.getPath(), server);
	if (!loc)
	{
		_setStatus(404, "Not Found");
		const std::map<int, std::string> & pages = server->getErrorPages();
		if (pages.find(404) != pages.end())
			_setBody(readFile(pages.at(404)));
		else
			_setBody("No matching location");
		return ;
	}
	_setBody(readFile(loc->getIndex()[0]));
}

std::string	Response::reconstitution( const Request & req, const Server * server ) const
{
	std::stringstream response;
	const Location * loc = _findLocation(req.getPath(), server);

	response << "HTTP/1.1 " << _status.first << " " << _status.second << "\r\n";

	if (_headers.find("Content-Type") == _headers.end())
	{
		std::string type = getContentType(loc->getIndex()[0]);
		response << "Content-Type: " << type << "\r\n";		
	}

	std::stringstream ss;
	ss << _body.size();
	response << "Content-Length: " << ss.str() << "\r\n";
	response << "Connection: keep-alive" << "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		response << it->first << ": " << it->second << "\r\n";
	response << "\r\n";
	response << _body;
	return response.str();
}

// Private Methods

const Location *	Response::_findLocation( const std::string & path, const Server * server ) const
{
	const std::vector<Location> & locations = server->getLocations();
	const Location * best = NULL;

	for (size_t i = 0; i < locations.size(); ++i)
		if (path.find(locations[i].getPath()) == 0)
			if (!best || locations[i].getPath().size() > best->getPath().size())
				best = &locations[i];
	return best;
}

std::string	Response::getContentType( const std::string & path ) const
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

// Setters

void Response::_setStatus( int status, const std::string & statusText ) { _status.first = status; _status.second = statusText; }
void Response::_setBody( const std::string & body ) { _body = body; }

/*
void Response::buildResponse(HttpRequest &req, const Server* server)
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
    int status = acstat_file(filePath.c_str(), F_OK | R_OK);
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
			int s = acstat_file(indexPath.c_str(), F_OK | R_OK);
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
}*/
