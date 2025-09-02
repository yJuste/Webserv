// ************************************************************************** //
//                                                                            //
//                configure_file.cpp                                          //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

#include "main.hpp"
#include <fstream>
#include <unistd.h>
#include <sstream>

bool	is_commentary( const std::string & line )
{
	if (line.empty())
		return true;
	if (line[0] == '#')
		return true;
	if (line.size() >= 2 && line[0] == '/' && line[1] == '/')
		return true;
	return false;
}

std::vector<std::string>	split( const std::string & s )
{
	std::stringstream		ss(s);
	std::string			word;
	std::vector<std::string>	words;

	while (ss >> word)
	{
		if (is_commentary(word))
			break ;
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

std::string	trim( const std::string & s )
{
	size_t	begin = s.find_first_not_of(" \t\r\n");
	if (begin == std::string::npos)
		return "";

	size_t	end = s.find_last_not_of(" \t\r\n");
	return s.substr(begin, end - begin + 1);
}

std::vector<Server *>	configure_file( const char * s )
{
	if (acstat(s, F_OK | R_OK) != 1)
		throw FailedAcstat(s);

	std::ifstream	file(s, std::ifstream::binary);
	if (!file.is_open())
		throw FailedOpen();

	std::string			line;
	std::vector<std::string>	words;

	while (std::getline(file, line))
	{
		line = trim(line);
		if (is_commentary(line))
			continue ;
		std::vector<std::string> w = split(line);
		words.insert(words.end(), w.begin(), w.end());
	}
	std::vector<Server *> servers = create_servers(words);
	return multi_ports(servers);
}
