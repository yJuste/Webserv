// ************************************************************************** //
//                                                                            //
//                Print.cpp                                                   //
//                Created on  : xxx Aug xx xx:xx:xx 2025                      //
//                Last update : xxx Aug xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Print.hpp"

void	Print::header( const std::string & title, const std::string & COLOR )
{
	int padding = (60 - title.size()) / 2;

	std::cout << COLOR;
	std::cout << ".............................................................." << std::endl;
	std::cout << "." << std::string(padding, ' ') << title << std::string(padding, ' ') << "." << std::endl;
	std::cout << "..............................................................";
	Print::endl(); Print::endl();
}

void	Print::subPart( const std::string & part, const std::string & COLOR )
{
	int padding = (57 - part.size());

	std::cout << std::endl << COLOR << "...";
	if (part != "")
		std::cout << " " << part << " ";
	std::cout << std::string(padding, '.');
	if (part == "")
		std::cout << std::string(2, '.');
	Print::endl();
}

void	Print::entry( const std::string & COLOR, const std::string & title )
{
	std::cout << COLOR << std::left << std::setw(20) << title;
}

void	Print::endl( void )
{
	std::cout << RESET << std::endl;
}
