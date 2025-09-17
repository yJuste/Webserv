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

// Dependences

# include "Server.hpp"
# include "utils.hpp"

class Client;
class Request;

/*	HELP
 *
 * Response Class find the nearest location possible.
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
		const Client *				_client;
		const Location *			_loc;

		std::pair<int, std::string>		_status;

		std::map<std::string, std::string>	_headers;
		std::string				_body;

		// Methods

		bool _preparation();
		void _reconstitution() const;

		// utils

		const Location * _findLocation() const;
		std::string getContentType( const std::string & ) const;
		bool allowsMethod( const std::string & ) const;

		// Setters

		void _setStatus( int, const std::string & );
		void _setBody( const std::string & );
		void _setHeader( const std::string &, const std::string & );

		Response();

	public:

		Response( Request * );
		~Response();

		Response( const Response & );
		Response & operator = ( const Response & );

		// Methods

		void build();	// construit la reponse.
		std::string string() const;	// convertit en string.
};

#endif
