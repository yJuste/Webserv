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

# include <iostream>
# include <vector>
# include <poll.h>

// Defines

# ifndef FDS_SIZE
#  define FDS_SIZE 200
# endif

// Dependences

# include "Server.hpp"

// ************************************************************************** //
//                                 Supervisor Class                           //
// ************************************************************************** //

class	Supervisor
{
	private:

		struct pollfd		_fds[FDS_SIZE];
		size_t			_size;

	public:

		Supervisor();
		Supervisor( const std::vector<Server> & );
		~Supervisor();

		Supervisor( const Supervisor & );
		Supervisor & operator = ( const Supervisor & );

		// Method

		void execution();

		// Getter

		size_t getSize() const;
		struct pollfd getFdsX( int ) const;
};

#endif
