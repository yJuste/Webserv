// ************************************************************************** //
//                                                                            //
//                configFile.cpp                                              //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"

// -----------------------------------------------------------------------------
std::vector<Server>	createServers( std::vector<std::string> & words );
// -----------------------------------------------------------------------------

std::vector<Server>	configFile( const char * s )
{
	std::ifstream		file;

	file.open(s, std::ifstream::binary);
	if (!file.is_open())
		throw FailedOpen();

	std::string		line;
	std::vector<std::string>	words;

	while (getline(file, line))
	{
		line = trim(line);
		if (is_commentary(line))
			continue ;
		std::vector<std::string>	words_splice = split(line);
		words.insert(words.end(), words_splice.begin(), words_splice.end());
	}

	std::vector<Server>	servers;

	servers = createServers(words);

	for (std::vector<std::string>::const_iterator it = words.begin(); it != words.end(); ++it)
		std::cout << *it << std::endl;

	file.close();
	return servers;
}

std::vector<Server>	createServers( std::vector<std::string> & words )
{
	std::vector<Server>	servers;
	std::vector<std::string>::iterator	it = words.begin();

	while (it != words.end())
	{
		if (*it == "server")
		{
			++it;
			if (it == words.end() || *it != "{")
				throw BracketsNotClosed();
			++it;

			Server		server;
			int		depth = 1;

			while (it != words.end() && depth > 0)
			{
				if (*it == "{")
					++depth;
				else if (*it == "}")
					--depth;
				else if (depth == 1)
				{
					if (++it == words.end())
						throw ValueNotGiven();
					setParameters(--it, server);
				}
				++it;
			}
			if (depth)
				throw BracketsNotClosed();
			servers.push_back(server);
		}
		else
			++it;
	}
	return servers;
}
