// ************************************************************************** //
//                                                                            //
//                main.hpp                                                    //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

#ifndef MAIN_HPP
# define MAIN_HPP

// Standard Libraries

# include <iostream>
# include <vector>
# include <sstream>

// ************************************************************************** //
//                                  Dependences                               //
// ************************************************************************** //

# include "Server.hpp"

// ************************************************************************** //
//                                  Prototypes                                //
// ************************************************************************** //

// parsing

std::list<Server>	configFile( const char * s );

std::string	trim( const std::string & );
bool	is_commentary( const std::string & );
std::list<std::string>	split( std::string & s );

#endif
