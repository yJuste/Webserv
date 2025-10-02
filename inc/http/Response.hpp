// ************************************************************************** //
//                                                                            //
//                Response.hpp                                                //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Layang                                        //
//                                                                            //
// ************************************************************************** //

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

// Standard Libraries

# include <signal.h>
# include <sys/wait.h>

// Dependences

# include "Server.hpp"
# include "SessionManager.hpp"
# include "utils.hpp"

class Client;
class Request;

// Defines

# ifndef USERS_FILE
#  define USERS_FILE ".users.db"
# endif

/*	HELP
 *
 * Response Class finds the nearest location possible.
 * Users are saved in USERS_FILE.
 *
 */

// ************************************************************************** //
//                               Response Class                               //
// ************************************************************************** //

class	Response
{
	private:

		const Request *				_req;
		const Server *				_server;
		Client *				_client;
		const Location *			_loc;
		SessionManager *			_smanager;
		Session *				_session;

		std::pair<int, std::string>		_status;
		std::map<std::string, std::string>	_headers;
		std::string				_body;

		// Methods

		int _preparation();
		void _reconstitution( const std::string & );
		void _handleGet( const std::string & );
		void _handlePost( const std::string & );
		void _handleUpload( std::string &, std::string & );
		void _registry( std::string & );
		void _handleDelete( const std::string &, const std::string & );
		void _executeCGI( const std::string & );
		int _session_management();
		void _check_keep_alive();

		// utils

		void _response( const std::string & );
		const Location * _findLocation( const std::string & ) const;
		bool _allowsMethod( const std::string & ) const;
		bool _autoIndex( const std::string & );
		bool _saveUploadedFile();
		bool _saveUser( const std::string &, const std::string &, const std::string & );
		bool _checkUser( const std::string &, const std::string & ) const;
		std::vector<std::string> _buildCgiEnv( const std::string & );
		void _apply_session_parameter();

		// ~Structor

		Response();

	public:

		Response( Request * );
		~Response();

		Response( const Response & );
		Response & operator = ( const Response & );

		// Methods

		void build();
		std::string string() const;

		// Getters

		const std::pair<int, std::string> & getStatus() const;
		const std::map<std::string, std::string> & getHeaders() const;
		std::string getHeader( const std::string & ) const;
		const std::string & getBody() const;
};

#endif
