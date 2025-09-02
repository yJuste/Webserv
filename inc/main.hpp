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

#include <vector>
#include <string>
#include "Exceptions.hpp"
class Server;
class Poller;
class HttpRequest;
class Location;

// Defines
# ifndef PATH_SIZE
#  define PATH_SIZE 256
# endif

# ifndef READ_SIZE
#  define READ_SIZE 4096
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
std::vector<Server *>	configure_file( const char * s);
// multi_ports.cpp
std::vector<Server *>	multi_ports( std::vector<Server *> & servers );

// init_server.cpp
std::vector<Server *>	create_servers( const std::vector<std::string> & words );

// init_location.cpp
Location	create_location( const std::vector<std::string> &, std::vector<std::string>::const_iterator &, Server & );

// utils.cpp
int	acstat( const char *, int );
void	stoa( const std::string & );
int	relative( const std::string & );
std::string	handle_folder( std::string s );

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

#endif
