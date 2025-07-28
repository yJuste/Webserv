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

std::vector<std::string>	split( const std::string & s )
{
	std::stringstream		ss(s);
	std::string			word;
	std::vector<std::string>	words;

	while (ss >> word)
	{
		if (word == "{}")
		{
			words.push_back("{");
			words.push_back("}");
		}
		else
			words.push_back(word);
	}
	return words;
}

bool	is_commentary( const std::string & line )
{
	if (line.empty())
		return true;
	if (line[0] == '#')
		return true;
	if (line[0] == '/' && line[1] == '/')
		return true;
	return false;
}

std::string	trim( const std::string & s )
{
	size_t	begin = s.find_first_not_of(" \t\r\n");
	size_t	end = s.find_last_not_of(" \t\r\n");

	if (begin == std::string::npos)
		return "";
	return s.substr(begin, end - begin + 1);
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

	std::vector<Server>	servers = create_servers(words);

	for (std::vector<std::string>::const_iterator it = words.begin(); it != words.end(); ++it)
		std::cout << *it << std::endl;

	file.close();
	return servers;
}
