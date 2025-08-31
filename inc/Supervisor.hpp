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

// Defines

# ifndef FDS_SIZE
#  define FDS_SIZE 200
# endif

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

// Dependences

# include "Server.hpp"
# include "Client.hpp"

/*	HELP

* The Supervisor class waits for an array of Servers allocated on the heap.
* Warning: supervisor destroys the vector<Server *> itself ( no need to delete[] )
* Test it with : curl -v http://IPv4:Port
* Test leaks : add cycles in execution() ( ~10 seconds to test ):

	size_t cycles = 0;
	while (_running && cycles++ < 1000000) { ... }

*/

// ************************************************************************** //
//                                 Supervisor Class                           //
// ************************************************************************** //

class	Supervisor
{
	private:

		struct pollfd		_fds[FDS_SIZE];
		size_t			_size;
		size_t			_server_size;

		std::vector<Server *>	_servers;
		std::vector<Client *>	_clients;

		bool			_find( const std::vector<Server *> &, int );
		bool			_supClient( int );
		void			_clean();

		Supervisor( const Supervisor & );
		Supervisor & operator = ( const Supervisor & );

	public:

		Supervisor();
		Supervisor( const std::vector<Server *> & );
		~Supervisor();

		// Methods

		void hold( const std::vector<Server *> & );
		void execution();

		// Getter

		size_t getSize() const;

		// Setters

		void addClient( Client * );

		// Exceptions

		class NoServerAdded;
		class SupNoClient;
};

class	Supervisor::NoServerAdded : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: Supervisor cannot monitor without a server."; } };

class	Supervisor::SupNoClient : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: Cannot find the client to suppress."; } };

#endif
