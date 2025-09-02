// ************************************************************************** //
//                                                                            //
//                Client.hpp                                                  //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

#ifndef CLIENT_HPP
# define CLIENT_HPP

// Standard Libraries

# include <iostream>
# include <netdb.h>
# include <unistd.h>
# include <fcntl.h>

// Dependences

# include "Exceptions.hpp"

/*	HELP
 *
 * Client is a RAII class.
 * Warning: Copying is forbidden because it's dangerous to duplicate a fd.
 *
 */

// ************************************************************************** //
//                                 Client Class                               //
// ************************************************************************** //

class	Client
{
	private:

		int		_socket;

		void		_unit( int );
		void		_backout();

		Client();

		Client( const Client & );
		Client & operator = ( const Client & );

	public:

		Client( int );
		~Client();

		// Getter

		int getSocket() const;
};

#endif
