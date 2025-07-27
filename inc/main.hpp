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
# include <iomanip>
# include <sys/stat.h>
# include <dirent.h>

// ************************************************************************** //
//                                  Dependences                               //
// ************************************************************************** //

# include "Server.hpp"

// ************************************************************************** //
//                                  Prototypes                                //
// ************************************************************************** //

// parsing

// configure_file.cpp
std::vector<Server>	configure_file( const char * );

// init_server.cpp
void	init_server( const std::vector<std::string> &, std::vector<std::string>::const_iterator &, Server & );

// init_location.cpp
void	init_location( const std::vector<std::string> &, std::vector<std::string>::const_iterator &, Server & );

// utils.cpp
std::string			trim( const std::string & );
bool				is_commentary( const std::string & );
std::vector<std::string>	split( const std::string & );
int				acstat( const char * path, int mode );

// debug.cpp

void	stoa( const std::string & );

#endif
