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

# include <sys/stat.h> 
# include <unistd.h>
# include <dirent.h>
# include <fstream>
# include <sstream>
# include <vector>
# include <algorithm>

// Dependences

# include "Server.hpp"

// ************************************************************************** //
//                                  Prototypes                                //
// ************************************************************************** //

// ----- Parsing -----

int	acstat( const char *, int );
int	relative( const std::string & );
std::string	handle_folder( std::string );

// ------ Http -------

std::string	my_getcwd();
std::string	readFile( const std::string & );
std::string	getContentType( const std::string & );
std::string	registryKey( const std::string &, const std::string & );
std::string	concatPaths( const std::string &, const std::string & );
std::string	remove_sub_string( const std::string &, const std::string & );
std::string	generateDirectoryListing( const std::string &, const std::string & );
void		replaceAll( std::string &, const std::string &, const std::string & );


// ------ Others -----

std::string	rounded( size_t );
std::string	getExtension( const std::string & );
std::string	url_decode( const std::string & );
std::string	to_clean( const std::string &, const std::string & );

// ----- Clients -----

void		finish_cgi( std::string & );
void		print_status_cgi( const std::string &, const std::string &, int );
bool		setBadGateway( std::string &, ssize_t &, const std::string & );

#endif
