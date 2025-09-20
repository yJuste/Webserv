// ************************************************************************** //
//                                                                            //
//                utils.hpp                                                   //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Layang                                        //
//                                                                            //
// ************************************************************************** //

#ifndef UTILS_HPP
# define UTILS_HPP

// Standard Libraries

# include <iostream>
# include <sys/stat.h> 
# include <iomanip>
# include <unistd.h>
# include <dirent.h>
# include <fstream>
# include <sstream>

// ************************************************************************** //
//                                  Prototypes                                //
// ************************************************************************** //

// ----- Parsing -----

int	acstat( const char *, int );
void	stoa( const std::string & );
int	relative( const std::string & );
std::string	handle_folder( std::string );

// ------ Http -------

std::string	readFile( const std::string & );
std::string	my_getcwd();
std::string	generateDirectoryListing( const std::string &, const std::string & );
std::string	getContentType( const std::string & );

// ------ Others -----

std::string	rounded( size_t );
std::string	getExtension( const std::string & );

#endif
