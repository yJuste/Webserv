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

const char * g_extensions[] = { ".py", ".php", NULL };
const char * g_methods[] = { "GET", "POST", "DELETE", NULL };

bool	dupCgi( const std::map<std::string, std::string> & cgi, const std::string & extension )
{
	std::map<std::string, std::string>::const_iterator it = cgi.find(extension);
	return it != cgi.end() && !it->second.empty();
}

void	init_cgi( std::vector<std::string>::const_iterator & it, Location & location )
{
	std::string	extension = *(it++);
	std::string	program = *it;

	if (program.empty() || program[program.size() - 1] != ';')
		throw NoEndingSemicolon();
	program.erase(program.size() - 1);
	if (acstat(program.c_str(), F_OK | X_OK) != 1)
		throw ProgramCgi(program.c_str());
	if (dupCgi(location.getCgi(), extension))
		throw DuplicateCgi(extension.c_str());

	location.addCgi(extension, program.c_str());
}

void	init_autoindex( std::string str, Location & location )
{
	if (str.empty() || str[str.size() - 1] != ';')
		throw NoEndingSemicolon();
	str.erase(str.size() - 1);
	if (str != "true" && str != "false"
		&& str != "1" && str != "0"
		&& str != "yes" && str != "no")
		throw InvalidAutoindex();

	location.setAutoindex(str == "true" || str == "1" || str == "yes");
	location.setOverwritten("autoindex");
}

void	init_index( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Location & location )
{
	location.getIndex().clear();
	while (it != words.end())
	{
		std::string	index = *it;
		bool		semicolon = false;

		if (!index.empty() && index[index.size() - 1] == ';')
		{
			index.erase(index.size() - 1);
			semicolon = true;
		}
		if (index.empty())
			throw NoEndingSemicolon();
		location.addIndex(index.c_str());
		if (semicolon)
			break ;
		++it;
	}
	if (it == words.end())
		throw NoEndingSemicolon();

	location.setOverwritten("index");
}

void	init_return( std::vector<std::string>::const_iterator & it, Location & location )
{

	std::stringstream	ss(*it);
	int			code;

	if (!(ss >> code))
		throw InvalidParameterReturn(it->c_str());
	std::string str = *(++it);
	if (str.empty() || str[str.size() - 1] != ';')
		throw NoEndingSemicolon();
	str.erase(str.size() - 1);

	location.setReturn(code, str.c_str());
	location.setOverwritten("return");
	location.setOverwritten("redirect");		// alias
}

void	init_methods( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Location & location )
{
	std::vector<std::string>	methods;
	std::vector<std::string>	ext;
	const int			count = (sizeof(g_methods) - sizeof(g_methods[0])) / sizeof(g_methods[0]);

	(location.getMethods()).clear();
	for ( int i = 0; i < count; ++i )
		ext.push_back(std::string(g_methods[i]));
	while (it != words.end())
	{
		std::string	method = *it;
		bool		semicolon = false;

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
	location.setOverwritten("allow_methods");	// alias
}

void	init_upload( std::string str, Location & location )
{
	if (str.empty() || str[str.size() - 1] != ';')
		throw NoEndingSemicolon();
	str.erase(str.size() - 1);

	location.getUpload().clear();

	location.setUpload(str.c_str());
	location.setOverwritten("upload");
}

void	init_root( std::string str, Location & location )
{
	if (str.empty() || str[str.size() - 1] != ';')
		throw NoEndingSemicolon();
	str.erase(str.size() - 1);

	location.getRoot().clear();

	location.setRoot(str.c_str());
	location.setOverwritten("root");
}

void	init_location( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server, Location & location )
{
	try { if (location.getOverwrittenX(*it)) throw OverwrittenParameterLocation(location.getPath().c_str(), it->c_str()); }
	catch ( std::exception & e ) { server.addWarning(e.what()); }

	if (*it == "methods" || *it == "allow_methods")
		init_methods(words, ++it, location);
	else if (*it == "return" || *it == "redirect")
		init_return(++it, location);
	else if (*it == "index")
		init_index(words, ++it, location);
	else if (*it == "autoindex")
		init_autoindex(*(++it), location);
	else if (*it == "cgi")
		init_cgi(++it, location);
	else if (*it == "upload")
		init_upload(*(++it), location);
	else if (*it == "root")
		init_root(*(++it), location);
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

void	create_paths( Location & location )
{
	location.setRoot(handle_folder(location.getRoot()));
	if (!location.getUpload().size())
		location.setUpload(handle_folder(location.getRoot()));
	else
		location.setUpload(handle_folder(location.getRoot() + location.getUpload()));

	bool status = false;

	std::vector<std::string> index = location.getIndex();
	for ( std::vector<std::string>::iterator it = index.begin(); it != index.end(); ++it )
	{
		*it = location.getRoot() + *it;
		if (acstat(it->c_str(), F_OK | R_OK) == 1)
			status = true;
	}
	if (!status)
		throw FailedAcstat(index[0].c_str());
	location.setIndex(index);
}

Location	create_location( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server )
{
	if (acstat(it->c_str(), F_OK | R_OK) != 2)
		throw FailedAcstat(it->c_str());

	Location	location;

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
		init_location(words, it, server, location);
		++it;
	}
	create_paths(location);
	return location;
}
