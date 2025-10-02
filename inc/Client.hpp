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

		int			_sv;
		ssize_t			_original;
		ssize_t			_total;

		// Methods

		void _unit( int );
		void _backout();

		// ~Structors

		Client();

		Client( const Client & );
		Client & operator = ( const Client & );

	public:

		Client( int, SessionManager * );
		~Client();

		// Methods

		int retrieve( const std::string & );
		int response();
		int receive( int, char *, size_t );
		ssize_t write();
		void sent();
		Server * select_server( const std::vector<Server *> & );

		// Getters

		int getSocket() const;
		const Server * getServer() const;
		SessionManager * getSessionManager() const;
		const char * getColor() const;
		std::string & wbuf();
		Request * getRequest();
		int getSv() const;

		// Setters

		void setServer( Server * );
		void setSv( int );
		void setOriginal( ssize_t );
};

#endif
