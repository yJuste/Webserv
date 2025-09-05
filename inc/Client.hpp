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

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "Server.hpp"

// Defines

# ifndef READ_SIZE
#  define READ_SIZE 4096
# endif

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
		std::string _readBuffer;
		std::string _writebBuffer;
		HttpRequest _request;
		Server* _server;

		void		_unit( int );
		void		_backout();

		Client();

		Client( const Client & );
		Client & operator = ( const Client & );

	public:

		Client(int server_socket, Server* server);
		~Client();

		// Getter

		int getSocket() const;
		bool hasDataToWrite() const;
		void readFromClient(const char * buffer, int n);
		void writeToClient();
};

#endif
