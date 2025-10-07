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

		ssize_t			_original;
		ssize_t			_total;

		int			_svRead;
		int			_svWrite;
		std::vector<char>	_cgiBody;
		size_t			_cgiSent;
		bool			_cgiSending;

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
		ssize_t receive( int, char *, size_t );
		ssize_t dispatch( int, const char *, size_t );
		ssize_t writing();
		void sent();
		Server * select_server( const std::vector<Server *> & );

		// Getters

		int getSocket() const;
		const Server * getServer() const;
		SessionManager * getSessionManager() const;
		const char * getColor() const;
		std::string & wbuf();
		Request * getRequest();
		int getSvRead() const;
		int getSvWrite() const;
		const std::vector<char> & getCgiBody() const;
		bool isCgiSending() const;

		// Setters

		void setServer( Server * );
		void setOriginal( ssize_t );
		void setSvRead( int );
		void setSvWrite( int );
		void setCgiBody( const std::vector<char> & body );
};

#endif
