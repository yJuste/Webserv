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

// configure_file.cpp
std::vector<Server>	configure_file( const char * s );

// init_server.cpp
void	init_server( std::vector<std::string> & words, std::vector<std::string>::iterator & it, Server & server );

// init_location.cpp
void	init_location( std::vector<std::string> & words, std::vector<std::string>::iterator & it, Server & server );

// utils.cpp
std::string	trim( const std::string & );
bool	is_commentary( const std::string & );
std::vector<std::string>	split( const std::string & s );

#endif
