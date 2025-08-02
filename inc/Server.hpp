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
# include <sstream>
# include <unistd.h>
# include <netdb.h>
# include <fstream>
# include <iomanip>
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

* Server is a RAII class.
* Warning: Copying is forbidden because it's dangerous to duplicate a fd.
* Startup() & Shutdown() can only be used once ( they are protected )

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
		int				_port;
		std::string			_root;
		bool				_default;
		std::vector<std::string>	_names;
		std::map<int, std::string>	_errorPages;
		size_t				_maxSize;
		std::vector<Location>		_locations;

		std::map<std::string, bool>	_duplicate;
		std::map<std::string, bool>	_overwritten;
		std::vector<std::string>	_warnings;

		std::string			_rounded( size_t ) const;

		Server( const Server & );
		Server & operator = ( const Server & );

	public:

		Server();
		~Server();

		// Methode

		void startup();			// start server (protected)
		void shutdown();		// close server (protected)
		void myConfig() const;		// print server configuration

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

		const std::map<std::string, bool> & getDuplicate() const;
		const std::map<std::string, bool> & getOverwritten() const;
		bool getDuplicateX( const std::string & ) const;
		bool getOverwrittenX( const std::string & ) const;

		const std::vector<std::string> & getWarnings() const;

		// Setter

		void setHost( const std::string & );
		void setPort( int );
		void setRoot( const std::string & );
		void setDefault( bool );
		void addName( const std::string & );
		void addErrorPage( int, const std::string & );
		void setMaxSize( int );
		void addLocation( const Location & );

		void setDuplicate( const std::string & );
		void setOverwritten( const std::string & );

		void addWarning( const std::string & );
};

#endif
