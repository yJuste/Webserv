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
* Warning: supervisor destroys the Servers itself ( no need to delete[] )

*/

// ************************************************************************** //
//                                 Supervisor Class                           //
// ************************************************************************** //

class	Supervisor
{
	private:

		struct pollfd		_fds[FDS_SIZE];
		size_t			_size;

		std::vector<Server *>	_servers;
		std::vector<Client *>	_clients;

		bool			_find( const std::vector<Server *> &, int );
		void			_supClient( int );
		void			_clean();

		Supervisor();

		Supervisor( const Supervisor & );
		Supervisor & operator = ( const Supervisor & );

	public:

		Supervisor( const std::vector<Server *> & );
		~Supervisor();

		// Method

		void execution();

		// Getter

		size_t getSize() const;
		struct pollfd getFdX( int ) const;

		// Setter

		void addClient( Client * );
};

#endif
