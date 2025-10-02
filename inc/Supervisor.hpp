// ************************************************************************** //
//                                                                            //
//                Supervisor.hpp                                              //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

#ifndef SUPERVISOR_HPP
# define SUPERVISOR_HPP

// Standard Libraries

# include <poll.h>

// Dependences

# include "Server.hpp"
# include "Client.hpp"
# include "SessionManager.hpp"

// Defines

# ifndef FDS_SIZE
#  define FDS_SIZE 200
# endif

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 16384
# endif

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

/*	HELP
 *
 * The Supervisor class waits for an array of Servers allocated on the heap.
 * Warning: supervisor destroys the vector<Server *> itself ( no need to delete[] )
 * Test it with :
 * curl -v http://IPv4:Port
 * Test virtual hosts :
 * curl -X GET --resolve lan:8090:127.0.0.1 http://lan:8090
 *
 */

// ************************************************************************** //
//                                 Supervisor Class                           //
// ************************************************************************** //

class	Supervisor
{
	private:

		struct pollfd		_fds[FDS_SIZE];
		std::vector<Server *>	_servers;
		std::vector<Client *>	_clients;
		SessionManager *	_smanager;

		size_t			_size;
		size_t			_server_size;
		size_t			_original_size;

		// Methods

		bool _supervise_stdin( bool & );
		void _new_client( int );
		void _reading( Client *, int, int );
		void _writing( Client *, int, int );

		// utils

		Client * _getClient( int );
		Client * _supClient( int );
		void _clock( bool &, time_t & );
		void _clean();

		// ~Structors

		Supervisor( const Supervisor & );
		Supervisor & operator = ( const Supervisor & );

	public:

		Supervisor();
		Supervisor( const std::vector<Server *> & );
		~Supervisor();

		// Methods

		void hold( const std::vector<Server *> & );
		void execution();

		// Exceptions

		class NoServerAdded;
};

class	Supervisor::NoServerAdded : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: Supervisor cannot monitor without a server."; } };

#endif
