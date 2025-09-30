// ************************************************************************** //
//                                                                            //
//                utils.cpp                                                   //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Layang                                        //
//                                                                            //
// ************************************************************************** //

# include "utils.hpp"

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

std::string	readFile( const std::string & path )
{
	std::ifstream file(path.c_str(), std::ifstream::binary);
	if (!file)
		return "";

	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
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

std::string	registryKey( const std::string & body, const std::string & key )
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

std::string	concatPaths( const std::string & path1, const std::string & path2 )
{
	if (path1.empty())
		return path2;
	if (path2.empty())
		return path1;

	size_t maxCheck = (path1.size() < path2.size()) ? path1.size() : path2.size();
	for (size_t len = maxCheck; len > 0; --len)
	{
		if (path1.compare(path1.size() - len, len, path2, 0, len) == 0)
		{
			std::string head = path1.substr(0, path1.size() - len);
			return head + path2;
		}
	}
	return path1 + path2;
}

std::string	remove_sub_string( const std::string & source, const std::string & to_remove )
{
	std::string res = source;
	if (to_remove.empty())
		return source;

	size_t pos = source.find(to_remove);
	if (pos != std::string::npos)
		res.erase(pos, to_remove.size());
	return res;
}

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
	html << "</ul></body></html>" << std::endl;
	closedir(dir);
	return html.str();
}

void	replaceAll( std::string & str, const std::string & from, const std::string & to )
{
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

std::string	url_decode( const std::string & src )
{
	std::string ret;
	for (size_t i = 0; i < src.size(); i++)
	{
		if (src[i] == '%' && i + 2 < src.size())
		{
			std::istringstream iss(src.substr(i + 1, 2));
			int hex = 0;
			iss >> std::hex >> hex;
			ret.push_back(static_cast<char>(hex));
			i += 2;
		}
		else if (src[i] == '+')
			ret.push_back(' ');
		else
			ret.push_back(src[i]);
	}
	return ret;
}
