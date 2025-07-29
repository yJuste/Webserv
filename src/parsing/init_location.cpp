// ************************************************************************** //
//                                                                            //
//                init_location.cpp                                           //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"

const char * g_extensions[] = { ".py", ".php", NULL };
const char * g_methods[] = { "GET", "POST", "DELETE", NULL };

void	init_cgi( std::vector<std::string>::const_iterator & it, Location & location )
{
	std::map<std::string, std::string>	cgi = location.getCgi();

	if (duplicate(cgi, g_extensions))
		throw ExtensionCgi();

	std::string	extension = *it;
	it++;
	std::string	program = *it;

	if (program.empty() || program.back() != ';')
		throw NoEndingSemicolon();
	program.pop_back();

	if (acstat(program.c_str(), F_OK | X_OK) != 1)
		throw ProgramCgi(program.c_str());

	for ( std::map<std::string, std::string>::const_iterator cit = cgi.begin(); cit != cgi.end(); ++cit )
		if (cit->first == extension && cit->second != "")
			throw DuplicateCgi(extension.c_str());

	location.addCgi(extension, actpath(program.c_str()));
}

void	init_autoindex( std::string str, Location & location )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	if (str != "true" && str != "false" && str != "1" && str != "0")
		throw InvalidAutoindex();

	location.setAutoindex(str == "true" || str == "1");
	location.setOverwritten("autoindex");
}

void	init_index( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Location & location )
{
	while (it != words.end())
	{
		std::string	index = *it;
		bool		semicolon = false;

		if (!index.empty() && index.back() == ';')
		{
			index.pop_back();
			semicolon = true;
		}

		if (index.empty())
			throw NoEndingSemicolon();
		if (acstat(index.c_str(), F_OK | R_OK) != 1)
			throw FailedAcstat(index.c_str());

		location.addIndex(actpath(index.c_str()));
		if (semicolon)
			break ;
		++it;
	}
	if (it == words.end())
		throw NoEndingSemicolon();
	location.setOverwritten("index");
}

void	init_return( std::string str, Location & location )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	if (acstat(str.c_str(), F_OK | R_OK) != 2)
		throw FailedAcstat(str.c_str());

	location.setReturn(actpath(str.c_str()));
	location.setOverwritten("return");
}

void	init_methods( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Location & location )
{
	std::vector<std::string>	methods;

	while (it != words.end())
	{
		std::string	method = *it;
		bool		semicolon = false;

		if (!method.empty() && method.back() == ';')
		{
			method.pop_back();
			semicolon = true;
		}
		if (method.empty())
			throw NoEndingSemicolon();

		std::vector<std::string>	ext;
		const int	count = (sizeof(g_methods) - sizeof(g_methods[0])) / sizeof(g_methods[0]);

		for ( int i = 0; i < count; ++i )
			ext.push_back(std::string(g_methods[i]));

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
	location.setDuplicate("methods");
	location.setDuplicate("allow_methods");
}

void	init_upload( std::string str, Location & location )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	if (acstat(str.c_str(), F_OK | R_OK) != 2)
		throw FailedAcstat(str.c_str());

	location.setUpload(actpath(str.c_str()));
	location.setOverwritten("upload");
}

void	init_location( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server, Location & location )
{
	try { if (location.getOverwrittenX(*it)) throw OverwrittenParameter(it->c_str()); }
	catch ( std::exception & e ) { server.addWarning(e.what()); }

	if (location.getDuplicateX(*it))
		throw DuplicateParameter(it->c_str());
	else if (*it == "methods" || *it == "allow_methods")
		init_methods(words, ++it, location);
	else if (*it == "return")
		init_return(*(++it), location);
	else if (*it == "index")
		init_index(words, ++it, location);
	else if (*it == "autoindex")
		init_autoindex(*(++it), location);
	else if (*it == "cgi")
		init_cgi(++it, location);
	else if (*it == "upload")
		init_upload(*(++it), location);
	else if (*it == "root")
		init_root(*(++it), server);
	else
		throw InvalidParameter(it->c_str());
}

void	create_location( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server )
{
	if (acstat(it->c_str(), F_OK | R_OK) != 2)
		throw FailedAcstat(it->c_str());

	Location	location;

	location.setPath(actpath(it->c_str()));

	std::vector<Location>	locations = server.getLocations();

	for (std::vector<Location>::const_iterator loc = locations.begin(); loc != locations.end(); ++loc)
	{
		if (loc->getPath() == location.getPath())
			throw DuplicateLocation((location.getPath()).c_str());
	}

	++it;
	if (it == words.end() || *it != "{")
		throw LocationNotGiven();
	++it;

	while (*it != "}")
	{
		if (*it == "{")
			throw BracketsNotClosed();
		if ((++it)-- == words.end())
			throw ValueNotGiven();
		init_location(words, it, server, location);
		++it;
	}
	server.addLocation(location);
}
