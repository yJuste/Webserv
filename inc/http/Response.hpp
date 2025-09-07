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

# include "Request.hpp"
# include "Server.hpp"
# include "utils.hpp"

// ************************************************************************** //
//                               Response Class                               //
// ************************************************************************** //

class	Response
{
	private:

		std::pair<int, std::string>		_status;

		std::map<std::string, std::string>	_headers;
		std::string				_body;

		// Methods

		const Location * _findLocation( const std::string &, const Server *) const;
		std::string getContentType( const std::string & ) const;

		// Setters

		void _setStatus( int, const std::string & );
		void _setBody( const std::string & );

	public:

		Response();
		~Response();

		Response( const Response & );
		Response & operator = ( const Response & );

		// Method

		void build( Request &, const Server * );
		std::string reconstitution( const Request &, const Server * ) const;
};

#endif
