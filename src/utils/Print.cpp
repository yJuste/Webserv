// ************************************************************************** //
//                                                                            //
//                Print.cpp                                                   //
//                Created on  : xxx Aug xx xx:xx:xx 2025                      //
//                Last update : xxx Aug xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Print.hpp"

const char * Print::_palette[g_palette_size];

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

const char *	Print::getColor( int socket )
{
	return _palette[static_cast<size_t>(socket) % (sizeof(_palette) / sizeof(_palette[0]))];
}

void	Print::newPalette( void )
{
	for (size_t i = 0; i < g_palette_size; ++i)
		_palette[i] = g_palette[i];
	for (size_t i = g_palette_size - 1; i > 0; --i)
	{
		size_t j = std::rand() % (i + 1);
		const char * tmp = _palette[i];
		_palette[i] = _palette[j];
		_palette[j] = tmp;
	}
}
