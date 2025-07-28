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

void	init_cgi( std::vector<std::string>::const_iterator & it, Location & location )
{
	if (*it != ".py" && *it != ".php")
		throw ExtensionCgi();

	std::string	extension = *it;
	it++;
	std::string	program = *it;

	if (program.empty() || program.back() != ';')
		throw NoEndingSemicolon();
	program.pop_back();

	if (acstat(program.c_str(), F_OK | X_OK) != 1)
		throw ProgramCgi(program.c_str());

	std::map<std::string, std::string>	cgi = location.getCgi();

	if (cgi.find(extension) != cgi.end())
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

void	init_default( std::string str, Location & location )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	if (acstat(str.c_str(), F_OK | R_OK) != 1)
		throw FailedAcstat(str.c_str());

	location.setDefault(actpath(str.c_str()));
	location.setOverwritten("default");
}

void	init_redirect( std::string str, Location & location )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	if (str != "true" && str != "false" && str != "1" && str != "0")
		throw InvalidRedirect();

	location.setRedirect(str == "true" || str == "1");
	location.setOverwritten("redirect");
}

void	init_methods( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Location & location )
{
	static std::vector<std::string>		av_meths;

	if (av_meths.empty())
	{
		av_meths.push_back("GET");
		av_meths.push_back("POST");
		av_meths.push_back("DELETE");
	}

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
		if (std::find(av_meths.begin(), av_meths.end(), method) == av_meths.end())
			throw MethodErrors();
		if (std::find(methods.begin(), methods.end(), method) != methods.end())
			throw MethodErrors();
		methods.push_back(method);
		if (semicolon)
			break ;
		++it;
	}
	location.setMethods(methods);
	location.setDuplicate("methods");
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

void	init_location( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Location & location )
{
	try { if (location.getOverwritten(*it)) throw OverwrittenParameter(it->c_str()); }
	catch ( std::exception & e ) { std::cout << e.what() << std::endl; }

	if (location.getDuplicate(*it))
		throw DuplicateParameter(it->c_str());
	else if (*it == "methods")
		init_methods(words, ++it, location);
	else if (*it == "redirect")
		init_redirect(*(++it), location);
	else if (*it == "default")
		init_default(*(++it), location);
	else if (*it == "autoindex")
		init_autoindex(*(++it), location);
	else if (*it == "cgi")
		init_cgi(++it, location);
	else if (*it == "upload")
		init_upload(*(++it), location);
	else
		throw InvalidParameter(it->c_str());
}

// duplicate root, path etc..
// default duplicate port server.
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
		init_location(words, it, location);
		++it;
	}
	if (location.getRedirect() && location.getDefault().empty())
		throw RedirXDefault();
	server.addLocation(location);
}
