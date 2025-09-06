// ************************************************************************** //
//                                                                            //
//                Print.hpp                                                   //
//                Created on  : xxx Aug xx xx:xx:xx 2025                      //
//                Last update : xxx Aug xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

#ifndef PRINT_HPP
# define PRINT_HPP

// Standard Libraries

# include <iostream>
# include <iomanip>
# include <sstream>

// Ansii

# ifndef BLUE
#  define BLUE "\033[38;5;74m"
# endif

# ifndef BEIGE
#  define BEIGE "\033[38;5;230m"
# endif

# ifndef YELLOW
#  define YELLOW "\033[38;5;220m"
# endif

# ifndef BROWN
#  define BROWN "\033[38;5;137m"
# endif

# ifndef APPLE_GREEN
#  define APPLE_GREEN "\033[38;5;154m"
# endif

# ifndef RED
#  define RED "\033[31m"
# endif

# ifndef RESET
#  define RESET "\033[0m"
# endif

# ifndef BOLD
#  define BOLD "\033[1m"
# endif

/*	HELP
 *
 * Print Class is for printing information or debug.
 *
 */

// ************************************************************************** //
//                                 Print Class                                //
// ************************************************************************** //

class	Print
{
	public:

		static void header( const std::string &, const std::string & );
		static void subPart( const std::string &, const std::string & );
		static void entry( const std::string &, const std::string & );
		static void endl();

		template <typename T> static std::ostream & value( const std::string &, const T & );
		template <typename T> static std::ostream & enval( const std::string &, const std::string &, const std::string &, const T & );
		template <typename T> static std::ostream & debug( const std::string &, const T &, const std::string & );
};

template <typename T>
std::ostream	& Print::enval( const std::string & COLOR1, const std::string & title, const std::string & COLOR2, const T & val )
{
	entry(COLOR1, title);
	value(COLOR2, val);
	Print::endl();
	return std::cout;
}

template <typename T>
std::ostream	& Print::value( const std::string & COLOR, const T & value )
{
	return std::cout << COLOR << value << RESET;
}

template <typename T>
std::ostream	& Print::debug( const std::string & COLOR, const T & type, const std::string & message )
{
	if (typeid(type) == typeid(int))
		return std::cout << "[" << COLOR << type << RESET << "]: " << message << std::endl;
	else
		return std::cout << COLOR << type << RESET << ": " << message << std::endl;
}

#endif
