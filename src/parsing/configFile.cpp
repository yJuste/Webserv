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
std::list<Server>	createServers( std::list<std::string> & words );
// -----------------------------------------------------------------------------

std::list<Server>	configFile( const char * s )
{
	std::ifstream		file;

	file.open(s, std::ifstream::binary);
	if (!file.is_open())
		throw FailedOpen();

	std::string		line;
	std::list<std::string>	words;

	while (getline(file, line))
	{
		line = trim(line);
		if (is_commentary(line))
			continue ;
		std::list<std::string>	words_splice = split(line);
		words.splice(words.end(), words_splice);
	}

	std::list<Server>	servers;

	servers = createServers(words);

	file.close();
	return servers;
}

std::list<Server>	createServers( std::list<std::string> & words )
{
	std::list<Server>	servers;
	std::list<std::string>::iterator	it = words.begin();

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
					if (*it == "listen")
					{
						if (++it == words.end())
							throw ListenNotGiven();
						server.setPort(std::atoi(it->c_str()));
					}
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
