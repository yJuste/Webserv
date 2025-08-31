// ************************************************************************** //
//                                                                            //
//                Server.hpp                                                  //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

#ifndef SERVER_HPP
# define SERVER_HPP

// Standard Libraries

# include <iostream>
# include <iomanip>
# include <sstream>
# include <fstream>
# include <cstring>
# include <unistd.h>
# include <netdb.h>
# include <fcntl.h>

// Dependences

# include "Location.hpp"

// Ansii

# ifndef BLUE
#  define BLUE "\033[38;5;74m"
# endif

# ifndef BEIGE
#  define BEIGE "\033[38;5;230m"
# endif

# ifndef YELLOW
#  define YELLOW "\033[38;5;220m"
# endif

# ifndef BROWN
#  define BROWN "\033[38;5;137m"
# endif

# ifndef GREEN
#  define GREEN "\033[38;5;154m"
# endif

# ifndef RESET
#  define RESET "\033[0m"
# endif

# ifndef BOLD
#  define BOLD "\033[1m"
# endif

/*	HELP
 *
 * Server is a RAII class.
 *
 * Startup() assigns a socket.
 * Startup() & Shutdown() can only be used once ( they are protected ).
 *
 * Copying a Server does not copy the socket.
 *
*/

// ************************************************************************** //
//                                 Server Class                               //
// ************************************************************************** //

class	Server
{
	private:

		int				_socket;
		struct sockaddr_in		_address;

		std::string			_host;
		std::vector<int>		_port;
		std::string			_root;
		bool				_default;
		std::vector<std::string>	_index;
		std::vector<std::string>	_names;
		std::map<int, std::string>	_errorPages;
		size_t				_maxSize;
		std::vector<Location>		_locations;

		// parsing

		std::map<std::string, bool>	_overwritten;
		std::vector<std::string>	_warnings;
		std::string			_rounded( size_t ) const;

	public:

		Server();
		~Server();

		Server( const Server & );
		Server & operator = ( const Server & );

		// Methods

		void startup();		// start server (protected)
		void shutdown();	// close server (protected)
		void myConfig() const;	// print server configuration

		// Getters

		int getSocket() const;
		const struct sockaddr_in & getAddress() const;
		const std::string & getHost() const;
		const std::vector<int> & getPort() const;
		int getFirstPort() const;
		bool getDefault() const;
		const std::vector<std::string> & getIndex() const;
		const std::vector<std::string> & getNames() const;
		const std::map<int, std::string> & getErrorPages() const;
		size_t getMaxSize() const;
		const std::string & getRoot() const;
		const std::vector<Location> & getLocations() const;
		const std::map<std::string, bool> & getOverwritten() const;
		bool getOverwrittenX( const std::string & ) const;
		const std::vector<std::string> & getWarnings() const;

		// Setters

		void setHost( const std::string & );
		void setPort( const std::vector<int> & );
		void addPort( int );
		void setRoot( const std::string & );
		void setDefault( bool );
		void setIndex( const std::vector<std::string> & );
		void setNames( const std::vector<std::string> & );
		void addErrorPage( int, const std::string & );
		void setMaxSize( int );
		void addLocation( const Location & );
		void setOverwritten( const std::string & );
		void addWarning( const std::string & );
};

#endif
