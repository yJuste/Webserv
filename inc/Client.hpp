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

// Dependences

# include "Server.hpp"
# include "Exceptions.hpp"
# include "Request.hpp"
# include "Response.hpp"

/*	HELP
 *
 * Client is a RAII class.
 * Warning: Copying is dangerous, be careful to have an unique fd.
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
		const char *		_color;

		std::string		_wbuf;
		Request *		_request;

		void _unit( int );
		void _backout();

		Client();

		Client( const Client & );
		Client & operator = ( const Client & );

	public:

		Client( int, Server * );
		~Client();

		// Methods

		void read( const std::string & );	// lit une requete.
		void write();	// envoie la requete.

		// Getters

		int getSocket() const;
		const Server * getServer() const;
		const char * getColor() const;
};

#endif
