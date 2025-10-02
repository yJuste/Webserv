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

# include <stdint.h>
# include <algorithm>

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
//                             Tests for webserv                              //
// ************************************************************************** //

/*
 *	SIEGE
 *
 * -- Pour que siege marche, il lui faut donner plus de ports, ainsi que reduire la duree du TIME_WAIT
 * Sur Macos:
 *	sudo sysctl -w net.inet.ip.portrange.first=10000
 *	sudo sysctl -w net.inet.ip.portrange.last=65535
 *	sudo sysctl -w net.inet.tcp.msl=1000
 * Sur Linux:
 *	sudo sysctl -w net.ipv4.ip_local_port_range="10000 65535"
 *	sudo sysctl -w net.ipv4.tcp_fin_timeout=10
 *
 * -> valeur de base: ( restauré après un reboot )
 *	first: 49152
 *	last: 65535
 *	port_range: 15000
 * Sur Linux:
 *	first: 32768
 *	last: 60999
 *	port_range (do): `sysctl net.ipv4.tcp_fin_timeout`
 */

/*
 *	Curl Tests:
 *
 * -- Open with curl :
 *	curl -v -X [method] http://IPv4:Port
 *
 * -- Tests virtual hosts (give server_name = lan):
 *	curl -X GET --resolve lan:8090:127.0.0.1 http://lan:8090
 *
 * -- Autoindex:
 *	curl -v http://127.0.0.1:8080 -H "Connection: close"
 *
 * -- Too large file:
 *	head -c 2000000 /dev/zero | curl -X POST \
 *	-H "Content-Type: text/plain" \
 *	--data-binary @- http://127.0.0.1:8090/login
 *
 */

// ************************************************************************** //
//                                  Prototypes                                //
// ************************************************************************** //

// ----- Main --------

void	create_unique_program();

// ----- Parsing -----

std::vector<Server *>	configure_file( const char *, std::vector<Server *> & );
std::vector<Server *>	multi_ports( std::vector<Server *> & );
std::vector<Server *>	create_servers( const std::vector<std::string> &, std::vector<Server *> & );
Location		create_location( const std::vector<std::string> &, std::vector<std::string>::const_iterator &, Server & );

#endif
