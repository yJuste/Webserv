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

# include <map>
# include <sstream>
# include <cstdlib>
# include <vector>
# include <algorithm>

// Dependences

class Client;

/*	HELP
 *
 * Need a Client class to work.
 *
 */

// ************************************************************************** //
//                                Request Class                               //
// ************************************************************************** //

class	Request
{
	private:

		Client *				_client;

		std::string				_headerPart;
		std::vector<char>			_body;
		std::string				_method;
		std::string				_path;
		std::string				_version;
		std::string				_query;
		std::string				_rawBuf;
		std::map<std::string, std::string>	_headers;
		bool					_unchunked;
		bool					_printed;

		// Methods

		bool _isComplete();
		std::vector<char> _unchunkBody( const std::vector<char> & );

		// ~Structor

		Request();

	public:

		Request( Client * );
		~Request();

		Request( const Request & );
		Request & operator = ( const Request & );

		// Methods

		int create( const std::string & );
		void reset();

		// Getters

		Client * getClient() const;
		const std::string & getHeaderPart() const;
		const std::vector<char> & getBody() const;
		const std::string & getMethod() const;
		const std::string & getPath() const;
		const std::string & getVersion() const;
		const std::string & getQuery() const;
		const std::map<std::string, std::string> & getHeaders() const;
		std::string getHeader( const std::string & ) const;
		bool getUnchunked() const;
		bool getPrinted() const;
};

#endif
