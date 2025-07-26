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

void	init_location( std::vector<std::string> & words, std::vector<std::string>::iterator & it, Server & server )
{
	std::string	str;

	str = *it;
	++it;
	if (it == words.end() || *it != "{")
		throw LocationNotGiven();
	++it;

	Location	location;

	while (*it != "}")
	{
		if (*it == "{")
			throw BracketsNotClosed();
		if ((++it)-- == words.end())
			throw ValueNotGiven();
		// setLocationParams
		++it;
	}
	(void)server;
	// server.setLocation();
}
