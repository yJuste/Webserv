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
# include <vector>
# include <map>
# include <unistd.h>
# include <netdb.h>
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
		std::string			_root;
		bool				_default;
		std::vector<std::string>	_names;
		std::map<int, std::string>	_errorPages;
		size_t				_maxSize;
		std::vector<Location>		_locations;

	public:

		Server();
		~Server();

		Server( const Server & );
		Server & operator = ( const Server & );

		// Methode

		void startup();
		void shutdown() const;

		// Getter

		int getSocket() const;
		struct sockaddr_in getAddress() const;

		std::string getHost() const;
		int getPort() const;
		bool getDefault() const;
		std::vector<std::string> getNames() const;
		std::map<int, std::string> getErrorPages() const;
		size_t getMaxSize() const;
		std::string getRoot() const;
		std::vector<Location> getLocations() const;

		std::string getNameX( int ) const;
		std::string getErrorPageX( int ) const;
		Location getLocationX( int ) const;

		// Setter

		void setHost( std::string & );
		void setPort( int );
		void setRoot( std::string & );
		void setDefault( bool );
		void setNames( std::vector<std::string> & );
		void setErrorPages( std::map<int, std::string> & );

		class Exception;
};

class	Server::Exception : public std::exception
{
	public : const char * what() const throw() { return "\033[31merror\033[0m: Server Error"; }
};

#endif
