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

/*	HELP
 *
 * The Supervisor class waits for an array of Servers allocated on the heap.
 * Warning: supervisor destroys the vector<Server *> itself ( no need to delete[] )
 * Test it with : curl -v http://IPv4:Port
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

		// Methods

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
