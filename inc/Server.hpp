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
		const struct sockaddr_in & getAddress() const;

		const std::string & getHost() const;
		int getPort() const;
		bool getDefault() const;
		const std::vector<std::string> & getNames() const;
		const std::map<int, std::string> & getErrorPages() const;
		size_t getMaxSize() const;
		const std::string & getRoot() const;
		const std::vector<Location> & getLocations() const;

		// Setter

		void setHost( const std::string & );
		void setPort( int );
		void setRoot( const std::string & );
		void setDefault( bool );
		void setNames( const std::vector<std::string> & );
		void setErrorPages( const std::map<int, std::string> & );
		void setMaxSize( int );
		void addLocation( const Location & );
};

#endif
