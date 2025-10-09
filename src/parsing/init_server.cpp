// ************************************************************************** //
//                                                                            //
//                init_server.cpp                                             //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"

void	init_max_size( std::string str, Server & server )
{
	if (str.empty() || str[str.size() - 1] != ';')
		throw NoEndingSemicolon();
	str.erase(str.size() - 1);

	char suff = str[str.size() - 1];
	std::string number = str;
	size_t mult = 1;
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

	std::stringstream ss(number);
	long long nb_signed;
	if (!(ss >> nb_signed) || !ss.eof())
		throw MaxSizeNotGiven();
	if (nb_signed < 0)
		throw MaxSizeNegative();

	size_t nb = static_cast<size_t>(nb_signed);
	if (nb > SIZE_MAX / mult)
		throw Overflow();
	server.setMaxSize(nb * mult);
	server.setOverwritten("client_max_body_size");
}

void	init_error_pages( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server )
{
	std::vector<int> codes;
	while (it != words.end())
	{
		std::stringstream ss(*it);
		int code;
		if (ss >> code && ss.eof())
		{
			codes.push_back(code);
			if (++it == words.end())
				throw ValueNotGiven();
			continue ;
		}

		std::string path = *it;
		bool semicolon = !path.empty() && path[path.size() - 1] == ';';
		if (semicolon)
			path.erase(path.size() - 1);
		if (path.empty())
			throw NoEndingSemicolon();
		for (std::vector<int>::const_iterator it = codes.begin(); it != codes.end(); ++it)
			server.addErrorPage(*it, path.c_str());
		if (semicolon)
			break ;
		++it;
	}
	if (it == words.end())
		throw NoEndingSemicolon();
}

void	init_names( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server )
{
	std::vector<std::string> names = server.getNames();
	while (it != words.end())
	{
		std::string name = *it;
		if (!name.empty() && name[name.size() - 1] == ';')
		{
			name.erase(name.size() - 1);
			if (name.empty())
				throw NoEndingSemicolon();
			names.push_back(name);
			break ;
		}
		names.push_back(name);
		++it;
	}
	if (it == words.end())
		throw NoEndingSemicolon();
	server.setNames(names);
}

void	init_index( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server )
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
	server.setIndex(tabs);
	server.setOverwritten("index");
}

void	init_root( std::string str, Server & server )
{
	if (str.empty() || str[str.size() - 1] != ';')
		throw NoEndingSemicolon();
	str.erase(str.size() - 1);
	if (server.getRoot() != "")
		throw DuplicateParameter("root");
	server.setRoot(str.c_str());
	server.setOverwritten("root");
}

void	init_listen( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server )
{
	std::vector<int> ports;
	while (it != words.end())
	{
		std::string str = *it;
		bool semicolon = false;
		if (!str.empty() && str[str.size() - 1] == ';')
		{
			str.erase(str.size() - 1);
			semicolon = true;
		}
		if (str.empty())
			throw NoEndingSemicolon();

		size_t sep = str.find(':');
		std::string host = "";
		std::string port = "";
		if (sep != std::string::npos)
		{
			host = str.substr(0, sep);
			if (host == "localhost")
				host = "127.0.0.1";
			port = str.substr(sep + 1);
			server.setHost(host);
			server.setOverwritten("host");
		}
		else
			port = str;

		std::stringstream ss(port);
		int nb;
		if (!(ss >> nb))
			throw InvalidListen();
		ports.push_back(nb);
		if (semicolon)
			break ;
		++it;
	}
	if (it == words.end())
		throw NoEndingSemicolon();
	server.setPort(ports);
}

void	init_host( std::string str, Server & server )
{
	if (str.empty() || str[str.size() - 1] != ';')
		throw NoEndingSemicolon();
	str.erase(str.size() - 1);
	if (str == "localhost")
		str = "127.0.0.1";
	server.setHost(str);
	server.setOverwritten("host");
}

void	init_server( const std::vector<std::string> & words, std::vector<std::string>::const_iterator & it, Server & server )
{
	if (*it == "host")
		init_host(*(++it), server);
	else if (*it == "listen")
		init_listen(words, ++it, server);
	else if (*it == "root")
		init_root(*(++it), server);
	else if (*it == "index")
		init_index(words, ++it, server);
	else if (*it == "server_name" || *it == "server_names") 
		init_names(words, ++it, server);
	else if (*it == "error_page")
		init_error_pages(words, ++it, server);
	else if (*it == "client_max_body_size")
		init_max_size(*(++it), server);
	else if (*it == "location")
		server.addLocation(create_location(words, ++it, server));
	else
		throw InvalidParameter(it->c_str());
}

void	missingImportant( Server & server )
{
	if (server.getHost() == "")
		server.setHost("0.0.0.0");
	try { if (server.getRoot() == "") throw MissingImportantValues("root"); }
	catch (std::exception & e) { server.addWarning(e.what()); }
	try { if (server.getLocations().empty()) throw MissingImportantValues("location"); }
	catch (std::exception & e) { server.addWarning(e.what()); }
	if (server.getNames().empty())
	{
		std::vector<std::string> name;
		name.push_back("localhost");
		server.setNames(name);
	}
	if (server.getEveryPort().empty())
		throw NoExistingPort();
}

void	create_paths( Server & server )
{
	server.setRoot(handle_folder(server.getRoot()));

	std::map<int, std::string> errors = server.getErrorPages();
	for (std::map<int, std::string>::const_iterator it = errors.begin(); it != errors.end(); ++it)
	{
		std::string str = it->second;
		int i = relative(str);
		std::string page = server.getRoot() + str.substr(i);
		server.addErrorPage(it->first, page);
		if (acstat(page.c_str(), F_OK | R_OK) != 1)
			throw FailedAcstat(page.c_str());
	}

	bool status = false;
	std::string first = "";
	std::vector<std::string> index = server.getIndex();
	for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); ++it)
	{
		*it = server.getRoot() + *it;
		if (!status && acstat(it->c_str(), F_OK | R_OK) == 1)
		{
			first = *it;
			status = true;
		}
	}
	if (index.empty())
		index.push_back(server.getRoot() + "index.html");
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
	server.setIndex(index);
}

void	overwritten( Server & server )
{
	std::map<std::string, int>::const_iterator cit = server.getOverwritten().begin();
	for (; cit != server.getOverwritten().end(); ++cit)
	{
		try { if (cit->second >= 2) throw OverwrittenParameter(cit->first.c_str()); }
		catch (std::exception & e) { server.addWarning(e.what()); }
	}
}

std::vector<Server *>	create_servers( const std::vector<std::string> & words, std::vector<Server *> & servers )
{
	std::vector<std::string>::const_iterator it = words.begin();
	while (it != words.end())
	{
		if (*it == "server")
		{
			if (++it == words.end() || *it != "{")
				throw BracketsNotClosed();
			if (++it == words.end())
				throw ValueNotGiven();

			Server * server = new Server();
			servers.push_back(server);
			while (*it != "}")
			{
				if (*it == "{")
					throw BracketsNotClosed();
				if ((++it)-- == words.end())
					throw ValueNotGiven();
				init_server(words, it, *server);
				++it;
			}
			missingImportant(*server);
			overwritten(*server);
			create_paths(*server);
		}
		else
			++it;
	}
	return servers;
}
