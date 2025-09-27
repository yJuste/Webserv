// ************************************************************************** //
//                                                                            //
//                init_location.cpp                                           //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"
# include "Location.hpp"

const char * g_methods[] = { "GET", "POST", "DELETE", NULL };

void	init_cgi_path( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Location & location )
{
	while (it != words.end())
	{
		std::string path = *it;
		bool semicolon = false;
		if (!path.empty() && path[path.size() - 1] == ';')
		{
			path.erase(path.size() - 1);
			semicolon = true;
		}
		if (path.empty())
			throw NoEndingSemicolon();
		location.addCgiPath(path);
		if (semicolon)
			break ;
		++it;
	}
	if (it == words.end())
		throw NoEndingSemicolon();
	location.setOverwritten("cgi");
}

void	init_cgi_ext( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Location & location )
{
	while (it != words.end())
	{
		std::string index = *it;
		bool semicolon = false;
		if (!index.empty() && index[index.size() - 1] == ';')
		{
			index.erase(index.size() - 1);
			semicolon = true;
		}
		if (index.empty())
			throw NoEndingSemicolon();
		if (index[0] != '.')
			throw NotExtension(index.c_str());
		location.addCgiExt(index);
		if (semicolon)
			break ;
		++it;
	}
	if (it == words.end())
		throw NoEndingSemicolon();
	location.setOverwritten("cgi");
}

void	init_cgi( std::vector<std::string>::const_iterator & it, Location & location )
{
	std::string extension = *(it++);
	std::string program = *it;
	if (extension[0] != '.')
		throw NotExtension(extension.c_str());
	if (program.empty() || program[program.size() - 1] != ';')
		throw NoEndingSemicolon();
	program.erase(program.size() - 1);
	if (acstat(program.c_str(), F_OK | X_OK) != 1)
		throw ProgramCgi(program.c_str());
	location.addCgi(extension, program.c_str());
	location.setOverwritten("cgi");
}

void	init_index( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Location & location )
{
	std::vector<std::string> tabs;
	while (it != words.end())
	{
		std::string index = *it;
		if (!index.empty() && index[index.size() - 1] == ';')
		{
			index.erase(index.size() - 1);
			if (index.empty())
				throw NoEndingSemicolon();
			tabs.push_back(index.c_str());
			break ;
		}
		tabs.push_back(index.c_str());
		++it;
	}
	if (it == words.end())
		throw NoEndingSemicolon();
	location.setIndex(tabs);
	location.setOverwritten("index");
}

void	init_autoindex( std::string str, Location & location )
{
	if (str.empty() || str[str.size() - 1] != ';')
		throw NoEndingSemicolon();
	str.erase(str.size() - 1);
	if (str != "true" && str != "false"
		&& str != "1" && str != "0"
		&& str != "yes" && str != "no"
		&& str != "on" && str != "off")
		throw InvalidAutoindex();
	location.setAutoindex(str == "true" || str == "1" || str == "yes" || str == "on");
	location.setOverwritten("autoindex");
}

void	init_return( std::vector<std::string>::const_iterator & it, Location & location )
{
	std::stringstream ss(*it);
	int code;
	if (!(ss >> code))
		throw InvalidParameterReturn(it->c_str());
	std::string str = *(++it);
	if (str.empty() || str[str.size() - 1] != ';')
		throw NoEndingSemicolon();
	str.erase(str.size() - 1);
	location.setReturn(code, str.c_str());
	location.setOverwritten("redirect");
}

void	init_methods( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Location & location )
{
	std::vector<std::string> methods;
	std::vector<std::string> ext;
	const int count = (sizeof(g_methods) - sizeof(g_methods[0])) / sizeof(g_methods[0]);
	for ( int i = 0; i < count; ++i )
		ext.push_back(std::string(g_methods[i]));
	while (it != words.end())
	{
		std::string method = *it;
		bool semicolon = false;
		if (!method.empty() && method[method.size() - 1] == ';')
		{
			method.erase(method.size() - 1);
			semicolon = true;
		}
		if (method.empty())
			throw NoEndingSemicolon();
		if (std::find(ext.begin(), ext.end(), method) == ext.end())
			throw MethodErrors();
		if (std::find(methods.begin(), methods.end(), method) != methods.end())
			throw MethodErrors();
		methods.push_back(method);
		if (semicolon)
			break ;
		++it;
	}
	if (it == words.end())
		throw NoEndingSemicolon();
	location.setMethods(methods);
	location.setOverwritten("methods");
}

void	init_upload( std::string str, Location & location )
{
	if (str.empty() || str[str.size() - 1] != ';')
		throw NoEndingSemicolon();
	str.erase(str.size() - 1);
	location.setUpload(str.c_str());
	location.setOverwritten("upload");
}

void	init_root( std::string str, Location & location )
{
	if (str.empty() || str[str.size() - 1] != ';')
		throw NoEndingSemicolon();
	str.erase(str.size() - 1);
	location.setRoot(str.c_str());
	location.setOverwritten("root");
}

void	init_location( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Location & location )
{
	if (*it == "root")
		init_root(*(++it), location);
	else if (*it == "upload" || *it == "upload_store")
		init_upload(*(++it), location);
	else if (*it == "methods" || *it == "allow_methods" || *it == "allowed_methods")
		init_methods(words, ++it, location);
	else if (*it == "return" || *it == "redirect")
		init_return(++it, location);
	else if (*it == "autoindex")
		init_autoindex(*(++it), location);
	else if (*it == "index")
		init_index(words, ++it, location);
	else if (*it == "cgi")
		init_cgi(++it, location);
	else if (*it == "cgi_ext")
		init_cgi_ext(words, ++it, location);
	else if (*it == "cgi_path")
		init_cgi_path(words, ++it, location);
	else
		throw InvalidParameter(it->c_str());
}

bool	dupLocation( const std::vector<Location> & locations, const std::string & path )
{
	for (std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it)
		if (it->getPath() == path)
			return true;
	return false;
}

void	create_paths( Location & location, Server & server )
{
	location.setRoot(handle_folder(location.getRoot()));
	if (!location.getUpload().size())
		location.setUpload(handle_folder(location.getRoot()));
	else
		location.setUpload(handle_folder(location.getRoot() + location.getUpload()));

	bool status = false;
	std::string first = "";
	std::vector<std::string> index = location.getIndex();
	for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); ++it)
	{
		*it = location.getRoot() + *it;
		if (!status && acstat(it->c_str(), F_OK | R_OK) == 1)
		{
			first = *it;
			status = true;
		}
	}
	if (index.empty())
		index.push_back(location.getRoot() + "index.html");
	else if (!status)
	{
		try { if (index.size()) throw FailedAcstat(index[0].c_str()); }
		catch (std::exception & e) { server.addWarning(e.what()); }
	}

	std::vector<std::string>::iterator pos = std::find(index.begin(), index.end(), first);
	if (pos != index.end())
	{
		index.erase(pos);
		index.insert(index.begin(), first);
	}
	location.setIndex(index);

	std::map<std::string, std::string> cgi = location.getCgi();
	std::vector<std::string> paths = location.getCgiPaths();
	std::map<std::string, std::string>::iterator it = cgi.begin();
	for (size_t i = 0; i < paths.size() && it != cgi.end(); ++i, ++it)
		it->second = paths[i];
	for (it = cgi.begin(); it != cgi.end();)
	{
		if (it->second == "")
		{
			std::map<std::string, std::string>::iterator tmp = it++;
			cgi.erase(tmp);
		}
		else
			++it;
	}
	location.setCgi(cgi);
}

void	overwritten( Location & location, Server & server )
{
	std::map<std::string, int>::const_iterator cit = location.getOverwritten().begin();
	for (; cit != location.getOverwritten().end(); ++cit)
	{
		try { if (cit->second >= 2) throw OverwrittenParameterLocation(location.getPath().c_str(), cit->first.c_str()); }
		catch (std::exception & e) { server.addWarning(e.what()); }
	}
}

Location	create_location( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server )
{
	Location location;
	location.setPath(it->c_str());
	location.setRoot(server.getRoot());

	std::vector<Location> locations = server.getLocations();
	if (dupLocation(server.getLocations(), location.getPath()))
		throw DuplicateLocation((location.getPath()).c_str());
	if (++it == words.end() || *(it++) != "{")
		throw LocationNotGiven();
	while (*it != "}")
	{
		if (*it == "{")
			throw BracketsNotClosed();
		if ((++it)-- == words.end())
			throw ValueNotGiven();
		init_location(words, it, location);
		++it;
	}
	create_paths(location, server);
	overwritten(location, server);
	return location;
}
