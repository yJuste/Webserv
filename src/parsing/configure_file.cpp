// ************************************************************************** //
//                                                                            //
//                configure_file.cpp                                          //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"

std::vector<Server>	createServers( const std::vector<std::string> & words )
{
	std::vector<Server>				servers;
	std::vector<std::string>::const_iterator	it;

	it = words.begin();
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
			Server	&server = servers.back();

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

std::vector<Server>	configure_file( const char * s )
{
	std::ifstream		file;

	if (acstat(s, F_OK | R_OK) != 1)
		throw FailedAcstat(s);
	file.open(s, std::ifstream::binary);
	if (!file.is_open())
		throw FailedOpen();

	std::string			line;
	std::vector<std::string>	words;

	while (std::getline(file, line))
	{
		line = trim(line);
		if (!is_commentary(line))
		{
			std::vector<std::string>	words_splice = split(line);
			words.insert(words.end(), words_splice.begin(), words_splice.end());
		}
	}

	std::vector<Server>	servers;

	servers = createServers(words);

	for (std::vector<std::string>::const_iterator it = words.begin(); it != words.end(); ++it)
		std::cout << *it << std::endl;

	file.close();
	return servers;
}
