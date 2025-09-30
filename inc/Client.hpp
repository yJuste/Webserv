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
# include "SessionManager.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Exceptions.hpp"

/*	HELP
 *
 * Client is a RAII class.
 * Warning: Copying is dangerous, be careful to have an unique fd.
 *
 * Autoindex test: curl -v http://127.0.0.1:8080 -H "Connection: close"
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
		SessionManager *	_smanager;

		const char *		_color;
		std::string		_wbuf;

		Request *		_request;

		// Methods

		void _unit( int );
		void _backout();

		// ~Structors

		Client();

		Client( const Client & );
		Client & operator = ( const Client & );

	public:

		Client( int, Server *, SessionManager * );
		~Client();

		// Methods

		void read( const std::string & );
		void write();

		// Getters

		int getSocket() const;
		const Server * getServer() const;
		SessionManager * getSessionManager() const;
		const char * getColor() const;
		bool getKeepAlive() const;
};

#endif
