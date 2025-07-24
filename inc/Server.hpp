// ************************************************************************** //
//                                                                            //
//                Server.hpp                                                  //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

#ifndef SERVER_HPP
# define SERVER_HPP

// Standard Libraries

# include <iostream>
# include <list>
# include <map>
# include <unistd.h>
# include <netinet/in.h>
# include <fstream>

// Dependences

# include "Location.hpp"

// ************************************************************************** //
//                                 Server Class                               //
// ************************************************************************** //

class	Server
{
	private:

		int				_socket;
		struct sockaddr_in		_address;

		std::string			_host;
		int				_port;
		bool				_default;
		std::list<std::string>		_names;
		std::map<int, std::string>	_errorPages;
		size_t				_maxSize;
		std::string			_root;
		std::list<Location>		_locations;

	public:

		Server();
		~Server();

		Server( const Server & );
		Server & operator = ( const Server & );

		// Methode

		void startup();
		void shutdown() const;

		// ~etter

		int getSocket() const;
		struct sockaddr_in getAddress() const;

		std::string getHost() const;
		int getPort() const;
		bool getDefault() const;
		std::string getNames( int ) const;
		std::string getErrorPages( int ) const;
		size_t getMaxSize() const;
		std::string getRoot() const;
		Location getLocation( int ) const;

		void setPort( int );

		class Exception;
};

class	Server::Exception : public std::exception
{
	public : const char * what() const throw() { return "\033[31merror\033[0m: Server Error"; }
};

#endif
