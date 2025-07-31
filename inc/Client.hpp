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

// ************************************************************************** //
//                                 Client Class                               //
// ************************************************************************** //

class	Client
{
	private:

		int		_socket;

	public:

		Client();
		~Client();

		Client( const Client & );
		Client & operator = ( const Client & );

		// Method

		void unit( int );
		void backout();

		// Getter

		int getSocket() const;
		const struct sockaddr_in & getAddress() const;
};

#endif
