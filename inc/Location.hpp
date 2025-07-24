// ************************************************************************** //
//                                                                            //
//                Location.hpp                                                //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

#ifndef LOCATION_HPP
# define LOCATION_HPP

// Standard Libraries

# include <iostream>

// ************************************************************************** //
//                                Location Class                              //
// ************************************************************************** //

class	Location
{
	private:

	public:

		Location();
		~Location();

		Location( const Location & );
		Location & operator = ( const Location & );

		class Exception;
};

class	Location::Exception : public std::exception
{
	public : const char * what() const throw() { return "\033[31merror\033[0m: Location Error"; }
};

#endif
