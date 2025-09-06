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
# include "Server.hpp"
# include "Request.hpp"

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

		int			_socket;
		Server *		_server;

		std::string		_wbuf;
		Request			_request;

		void _unit( int );
		void _clean();
		void _backout();

		Client();

		Client( const Client & );
		Client & operator = ( const Client & );

	public:

		Client( int, Server * );
		~Client();

		// Methods

		void read( const std::string & );
		void write();

		// Getter

		int getSocket() const;
};

#endif
