// ************************************************************************** //
//                                                                            //
//                utils.cpp                                                   //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Layang                                        //
//                                                                            //
// ************************************************************************** //

# include "utils.hpp"

std::string	rounded( size_t bytes )
{
	const char * units[] = {"bytes", "KB", "MB", "GB", "TB"};
	double size = static_cast<double>(bytes);
	int unit = 0;

	while ( size >= 1024.0 && unit < 4 )
	{
		size /= 1024.0;
		++unit;
	}

	std::stringstream	ss;

	ss.precision(2);
	ss.setf(std::ios::fixed);
	ss << size << " " << units[unit];
	return ss.str();
}

std::string	readFile( const std::string & path )
{
	std::ifstream file(path.c_str(), std::ifstream::binary);
	if (!file)
		return "";
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

std::string	my_getcwd( void )
{
	std::vector<std::string> parts;
	struct stat current, parent;

	if (stat(".", &current) < 0)
		return "";

	while (true)
	{
		if (stat("..", &parent) < 0)
			return "";
		if (current.st_ino == parent.st_ino && current.st_dev == parent.st_dev)
			break;
		if (chdir("..") < 0)
			return "";
		DIR *dir = opendir(".");
		if (!dir)
			return "";
		struct dirent * entry;
		std::string dirname;
		while ((entry = readdir(dir)) != NULL)
		{
			struct stat st;
			if (stat(entry->d_name, &st) == 0 && st.st_ino == current.st_ino && st.st_dev == current.st_dev)
			{
				dirname = entry->d_name;
				break;
			}
		}
		closedir(dir);
		parts.insert(parts.begin(), dirname);
		current = parent;
	}
	std::string path = "/";
	for (size_t i = 0; i < parts.size(); ++i)
	{
		path += parts[i];
		if (i + 1 < parts.size())
			path += "/";
	}
	if (!path.empty())
		chdir(path.c_str());
	return path;
}

// Generate directory Listing for autoindex

std::string	generateDirectoryListing( const std::string & dirPath, const std::string & reqPath )
{
	DIR * dir;
	struct dirent * entry;
	std::stringstream html;

	dir = opendir(dirPath.c_str());
	if (!dir)
		return "<html><body><h1>403 Forbidden</h1></body></html>";

	html << "<html><head><title>Index of " << reqPath << "</title></head><body>" << std::endl;
	html << "<h1>Index of " << reqPath << "</h1><ul>" << std::endl;

	while ((entry = readdir(dir)) != NULL)
	{
		std::string name(entry->d_name);
		if (name == ".")
			continue ;
		std::string href = reqPath;
		if (href[href.size() - 1] != '/')
			href += '/';
		href += name;
		html << "<li><a href=\"" << href << "\">" << name << "</a></li>" << std::endl;
	}
	closedir(dir);
	html << "</ul></body></html>" << std::endl;
	return html.str();
}

std::string	getContentType( const std::string & path )
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
