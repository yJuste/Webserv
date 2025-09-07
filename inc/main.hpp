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

// Dependences

# include "Server.hpp"
# include "Supervisor.hpp"
# include "utils.hpp"

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

// ----- Main --------

void	create_unique_program();

// ----- Parsing -----

// configure_file.cpp
std::vector<Server *>	configure_file( const char * );
std::vector<Server *>	multi_ports( std::vector<Server *> & );

// init_server.cpp
std::vector<Server *>	create_servers( const std::vector<std::string> & );

// init_location.cpp
Location	create_location( const std::vector<std::string> &, std::vector<std::string>::const_iterator &, Server & );

#endif
