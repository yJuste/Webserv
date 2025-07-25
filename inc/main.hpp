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

// configFile.cpp
std::vector<Server>	configFile( const char * s );

// setParams.cpp
void	setParameters( std::vector<std::string>::iterator & it, Server & server );

// utils.cpp
std::string	trim( const std::string & );
bool	is_commentary( const std::string & );
std::vector<std::string>	split( const std::string & s );

#endif
