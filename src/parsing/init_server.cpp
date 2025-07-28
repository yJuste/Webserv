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

void	init_max_size( std::string str, Server & server )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	char		suff = str[str.size() - 1];
	std::string	number = str;
	size_t		mult = 1;

	if (!std::isdigit(suff))
	{
		switch (suff)
		{
			case 'K' : case 'k' : mult = 1024;
				break ;
			case 'M' : case 'm' : mult = 1048576;
				break ;
			case 'G' : case 'g' : mult = 1073741824;
				break ;
			default :
				throw MaxSizeNotGiven();
		}
		number = str.substr(0, str.size() - 1);
	}

	std::stringstream	ss(number);
	size_t			nb;

	if (!(ss >> nb) || !ss.eof())
		throw MaxSizeNotGiven();
	if (nb > SIZE_MAX / mult)
		throw Overflow();

	server.setMaxSize(nb * mult);
	server.setDuplicate("client_max_body_size");
}

void	init_error_pages( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server )
{
	while (it != words.end())
	{
		std::stringstream	ss(*it);
		int			code;

		if (!(ss >> code) || !ss.eof())
			throw ErrorPageNotGiven();
		if (++it == words.end())
			throw ValueNotGiven();

		std::string	path = *it;
		bool		semicolon = !path.empty() && path.back() == ';';

		if (semicolon)
			path.pop_back();
		if (path.empty())
			throw NoEndingSemicolon();
		if (acstat(path.c_str(), F_OK | R_OK) == -1)
			throw FailedAcstat(path.c_str());
		server.addErrorPage(code, actpath(path.c_str()));
		if (semicolon)
			break ;
		++it;
	}
	if (it == words.end())
		throw NoEndingSemicolon();
	server.setOverwritten("error_page");
}

void	init_names( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server )
{
	while (it != words.end())
	{
		std::string	name = *it;

		if (!name.empty() && name.back() == ';')
		{
			name.pop_back();
			if (name.empty())
				throw NoEndingSemicolon();
			server.addName(name);
			break ;
		}
		server.addName(name);
		++it;
	}
	if (it == words.end())
		throw NoEndingSemicolon();
}

void	init_root( std::string str, Server & server )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	if (acstat(str.c_str(), F_OK | R_OK) != 2)
		throw FailedAcstat(str.c_str());

	server.setRoot(actpath(str.c_str()));
	server.setDuplicate("root");
}

void	init_host( std::string str, Server & server )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	server.setHost(str);
	server.setDuplicate("host");
}

void	init_listen( std::string str, Server & server )
{
	if (str.empty() || str.back() != ';')
		throw NoEndingSemicolon();
	str.pop_back();

	size_t		sep = str.find(':');
	std::string	host = "";
	std::string	port = "";

	if (sep != std::string::npos)
	{
		host = str.substr(0, sep);
		port = str.substr(sep + 1);

		server.setHost(host);
	}
	else
		port = str;

	std::stringstream	ss(port);
	int			nb;

	if (!(ss >> nb) || !ss.eof())
		throw InvalidListen();
	server.setPort(nb);
	if (sep != std::string::npos)
		server.setDuplicate("host");
	server.setDuplicate("listen");
}

void	init_server( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server )
{
	try { if (server.getOverwritten(*it)) throw OverwrittenParameter(it->c_str()); }
	catch ( std::exception & e ) { std::cout << e.what() << std::endl; }

	if (server.getDuplicate(*it))
		throw DuplicateParameter(it->c_str());
	else if (*it == "host")
		init_host(*(++it), server);
	else if (*it == "listen")
		init_listen(*(++it), server);
	else if (*it == "root")
		init_root(*(++it), server);
	else if (*it == "server_name")
		init_names(words, ++it, server);
	else if (*it == "error_page")
		init_error_pages(words, ++it, server);
	else if (*it == "client_max_body_size")
		init_max_size(*(++it), server);
	else if (*it == "location")
		create_location(words, ++it, server);
	else
		throw InvalidParameter(it->c_str());
}

std::vector<Server>	create_servers( const std::vector<std::string> & words )
{
	std::vector<Server>				servers;
	std::vector<std::string>::const_iterator	it = words.begin();

	while (it != words.end())
	{
		if (*it == "server")
		{
			++it;
			if (it == words.end() || *it != "{")
				throw BracketsNotClosed();
			if (++it == words.end())
				throw ValueNotGiven();

			servers.emplace_back();
			Server & server = servers.back();

			while (*it != "}")
			{
				if (*it == "{")
					throw BracketsNotClosed();
				if ((++it)-- == words.end())
					throw ValueNotGiven();
				init_server(words, it, server);
				++it;
			}
			if (servers.empty())
				server.setDefault(true);
		}
		else
			++it;
	}
	return servers;
}
