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

# include <algorithm>
# include <sys/stat.h>
# include <dirent.h>
# include <stdint.h>

// Dependences

<<<<<<< HEAD
# include "Exceptions.hpp"
#include "Poller.hpp"
=======
# include "Server.hpp"
>>>>>>> main
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

// ----- Main --------
<<<<<<< HEAD
=======

>>>>>>> main
void	create_unique_program();

// ----- Parsing -----

// configure_file.cpp
<<<<<<< HEAD
std::vector<Server *>	configure_file( const char * s);

// multi_ports.cpp
std::vector<Server *>	multi_ports( std::vector<Server *> & servers );

// init_server.cpp
std::vector<Server *>	create_servers( const std::vector<std::string> & words );

// ----- Parsing -----

// configure_file.cpp
=======
>>>>>>> main
std::vector<Server *>	configure_file( const char * );
std::vector<Server *>	multi_ports( std::vector<Server *> & );

// init_server.cpp
std::vector<Server *>	create_servers( const std::vector<std::string> & );

// init_location.cpp
Location	create_location( const std::vector<std::string> &, std::vector<std::string>::const_iterator &, Server & );

// utils.cpp
int	acstat( const char *, int );
void	stoa( const std::string & );
int	relative( const std::string & );
std::string	handle_folder( std::string s );

<<<<<<< HEAD
// ----- Responding -----

// file_utils.cpp

long getFileSize(const std::string &path);
std::string generateDirectoryListing(const std::string &path);
bool userExists(const std::string &username);
std::string getFormValue(const std::string &body, const std::string &key);
bool saveUser(const std::string &username, const std::string &password);
bool checkUser(const std::string &username, const std::string &password);
bool saveUploadedFile(const HttpRequest &req, const std::string &uploadDir);
std::string resolvePath(const Location* loc, const std::string &reqPath);
std::string readFile(const std::string &path);

=======
>>>>>>> main
#endif
