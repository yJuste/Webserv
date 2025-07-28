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

void	init_autoindex( std::string str, Location & location )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	if (str != "true" && str != "false" && str != "1" && str != "0")
		throw InvalidAutoindex();

	location.setAutoindex(str == "true" || str == "1");
}

void	init_default( std::string str, Location & location )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	if (acstat(str.c_str(), F_OK | R_OK) != 1)
		throw FailedAcstat(str.c_str());

	location.setDefault(str);
}

void	init_redirect( std::string str, Location & location )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	if (str != "true" && str != "false" && str != "1" && str != "0")
		throw InvalidRedirect();

	location.setRedirect(str == "true" || str == "1");
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

		bool	semicolon = false;
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
}

void	init_location( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Location & location )
{
	if (*it == "methods")
		init_methods(words, ++it, location);
	else if (*it == "redirect")
		init_redirect(*(++it), location);
	else if (*it == "default")
		init_default(*(++it), location);
	else if (*it == "autoindex")
		init_autoindex(*(++it), location);
	else
		throw InvalidParameter(it->c_str());
}

void	create_location( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server )
{
	if (acstat(it->c_str(), F_OK | R_OK) != 2)
		throw FailedAcstat(it->c_str());

	Location	location;

	location.setPath(*(it++));

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
