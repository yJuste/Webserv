// ************************************************************************** //
//                                                                            //
//                trim.cpp                                                    //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"

// trim whitespaces at beginning and end.
std::string	trim( const std::string & s )
{
	size_t	begin = s.find_first_not_of(" \t\r\n");
	size_t	end = s.find_last_not_of(" \t\r\n");

	if (begin == std::string::npos)
		return "";
	return s.substr(begin, end - begin + 1);
}

// skip empty line & commentaries.
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

// split a string.
std::vector<std::string>	split( const std::string & s )
{
	std::stringstream		ss(s);
	std::string			word;
	std::vector<std::string>	words;

	while (ss >> word)
		words.push_back(word);
	return words;
}
