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

# include <unistd.h>
# include <netdb.h>
# include <fcntl.h>
# include <vector>
# include <cstring>

// Dependences

# include "Location.hpp"
# include "Exceptions.hpp"
# include "Print.hpp"
# include "utils.hpp"

/*	HELP
 *
 * Server is a RAII class.
 *
 * Startup() assigns a socket.
 * Startup() & Shutdown() can only be used once ( they are protected ).
 *
 * Warning: Copying is dangerous, be careful to have an unique fd.
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
		std::vector<std::string>	_warnings;
		std::map<std::string, int>	_overwritten;

	public:

		Server();
		~Server();

		Server( const Server & );
		Server & operator = ( const Server & );

		// Methods

		void startup();		// (protected)
		void shutdown();	// (protected)
		void myConfig() const;	// prints the server configuration.

		// Getters

		int getSocket() const;
		const struct sockaddr_in & getAddress() const;
		const std::string & getHost() const;
		const std::vector<int> & getEveryPort() const;
		int getPort() const;
		bool getDefault() const;
		const std::vector<std::string> & getIndex() const;
		const std::vector<std::string> & getNames() const;
		const std::map<int, std::string> & getErrorPages() const;
		size_t getMaxSize() const;
		const std::string & getRoot() const;
		const std::vector<Location> & getLocations() const;
		const Location * getXLocation( const std::string & ) const;
		const std::map<std::string, int> & getOverwritten() const;
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
