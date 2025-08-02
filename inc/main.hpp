// ************************************************************************** //
//                                                                            //
//                main.hpp                                                    //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

#ifndef MAIN_HPP
# define MAIN_HPP

// Standard Libraries

# include <sys/stat.h>
# include <dirent.h>

// Dependences

# include "Server.hpp"
# include "Supervisor.hpp"

// Defines

# ifndef PATH_SIZE
#  define PATH_SIZE 256
# endif

# ifndef BUFFER
#  define BUFFER 256
# endif

// ************************************************************************** //
//                                  Prototypes                                //
// ************************************************************************** //

// ----- Parsing -----

// configure_file.cpp
std::vector<Server *>	configure_file( const char * );

// init_server.cpp
std::vector<Server *>	create_servers( const std::vector<std::string> & );
void	init_root( std::string str, Server & server );

// init_location.cpp
void	create_location( const std::vector<std::string> &, std::vector<std::string>::const_iterator &, Server & );

// utils.cpp
int	acstat( const char *, int );
std::string	actpath( const char * );
void	stoa( const std::string & );

#endif
