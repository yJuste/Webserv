// ************************************************************************** //
//                                                                            //
//                Request.hpp                                                 //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Layang                                        //
//                                                                            //
// ************************************************************************** //

#ifndef REQUEST_HPP
# define REQUEST_HPP

// Standard Libraries

# include <iostream>
# include <string>
# include <sstream>
# include <map>

// ************************************************************************** //
//                                Request Class                               //
// ************************************************************************** //

class	Request
{
	private:

		std::string				_headerPart;
		std::string				_body;

		std::string				_method;
		std::string				_path;
		std::string				_version;
		bool					_unchunked;
		bool					_printed;

		std::map<std::string, std::string>	_headers;

		std::string unchunkBody( const std::string & );
		void printCRLF( const std::string & );

	public:

		Request();
		~Request();

		Request( const Request & );
		Request & operator = ( const Request & );

		// Method

		void parse( const std::string & );
		bool isComplete();
		void reset();

		// Getters

		const std::string & getHeaderPart() const;
		const std::string & getBody() const;
		const std::string & getMethod() const;
		const std::string & getPath() const;
		const std::string & getVersion() const;
		const std::map<std::string, std::string> & getHeaders() const;
		bool getUnchunked() const;
		bool getPrinted() const;

		// Setter

		void setPrinted( bool );
};

#endif
