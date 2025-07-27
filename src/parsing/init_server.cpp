// ************************************************************************** //
//                                                                            //
//                init_server.cpp                                             //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"

// ----------------------------------------------------------------------------
void	init_listen( std::string & str, Server & server );
void	init_host( std::string & str, Server & server );
void	init_root( std::string & str, Server & server );
void	init_names( std::vector<std::string> & words, std::vector<std::string>::iterator & it, Server & server );
void	init_error_pages( std::vector<std::string> & words, std::vector<std::string>::iterator & it, Server & server );
// ----------------------------------------------------------------------------

void	init_server( std::vector<std::string> & words, std::vector<std::string>::iterator & it, Server & server )
{
	if (*it == "listen")
		init_listen(*(++it), server);
	else if (*it == "host")
		init_host(*(++it), server);
	else if (*it == "root")
		init_root(*(++it), server);
	else if (*it == "server_name")
		init_names(words, ++it, server);
	else if (*it == "error_page")
		init_error_pages(words, ++it, server);
	else if (*it == "location")
		init_location(words, ++it, server);
	else
		throw InvalidParameter(it->c_str());
}

void	init_listen( std::string & str, Server & server )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	size_t		sep;

	sep = str.find(':');
	if (sep != std::string::npos)
	{
		std::string	host = str.substr(0, sep);
		std::string	port = str.substr(sep + 1);

		server.setHost(host);
		server.setPort(std::atoi(port.c_str()));
	}
	else
		server.setPort(std::atoi(str.c_str()));
}

void	init_host( std::string & str, Server & server )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	server.setHost(str);
}

void	init_root( std::string & str, Server & server )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	server.setRoot(str);
}

void	init_names( std::vector<std::string> & words, std::vector<std::string>::iterator & it, Server & server )
{
	std::vector<std::string>	names;

	while (it != words.end())
	{
		std::string	name = *it;

		if (!name.empty() && name.back() == ';')
		{
			name.pop_back();
			names.push_back(name);
			break ;
		}
		else
			names.push_back(name);
		++it;

	}
	if (names.empty())
		throw NoEndingSemicolon();
	server.setNames(names);
}

void	init_error_pages( std::vector<std::string> & words, std::vector<std::string>::iterator & it, Server & server )
{
	std::map<int, std::string>	errors;

	while (it != words.end())
	{
		std::stringstream	ss(*it);
		int			n;

		if (!(ss >> n) || !ss.eof())
			throw FailedErrorPage();
		if (++it == words.end())
			throw ValueNotGiven();

		std::string	path = *it;
		bool		semicolon = !path.empty() && path.back() == ';';

		if (semicolon)
			path.pop_back();
		if (acstat(path.c_str(), F_OK | R_OK) == -1)
			throw FailedAcstat(path.c_str());
		errors[n] = path;
		if (semicolon)
			break ;
		++it;
	}
	server.setErrorPages(errors);
}
