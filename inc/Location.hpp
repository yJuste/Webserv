// ************************************************************************** //
//                                                                            //
//                Location.hpp                                                //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

#ifndef LOCATION_HPP
# define LOCATION_HPP

// Standard Libraries

# include <string>
# include <vector>
# include <map>

// Global Variables

extern const char * g_methods[];

/*	HELP
 *
 * The Location class needs arrays for supported extensions and allowed methods.
 *
 */

// ************************************************************************** //
//                                Location Class                              //
// ************************************************************************** //

class	Location
{
	private:

		std::string				_path;
		std::string				_root;
		std::string				_upload;
		std::vector<std::string>		_methods;
		std::map<int, std::string>		_return;
		bool					_autoindex;
		std::vector<std::string>		_index;
		std::map<std::string, std::string>	_cgi;
		std::vector<std::string>		_cgi_paths;
		std::map<std::string, int>		_overwritten;

	public:

		Location();
		~Location();

		Location( const Location & );
		Location & operator = ( const Location & );

		// Getters

		const std::string & getPath() const;
		const std::string & getRoot() const;
		const std::string & getUpload() const;
		const std::vector<std::string> & getMethods() const;
		const std::map<int, std::string> & getReturn() const;
		bool getAutoindex() const;
		const std::vector<std::string> & getIndex() const;
		const std::map<std::string, std::string> & getCgi() const;
		const std::vector<std::string> & getCgiPaths() const;
		const std::map<std::string, int> & getOverwritten() const;

		// Setters

		void setPath( const std::string & );
		void setRoot( const std::string & );
		void setUpload( const std::string & );
		void setMethods( const std::vector<std::string> & );
		void setReturn( int, const std::string & );
		void setAutoindex( bool );
		void setIndex( const std::vector<std::string> & );
		void setCgi( const std::map<std::string, std::string> & );
		void addCgi( const std::string &, const std::string & );
		void addCgiExt( const std::string & );
		void addCgiPath( const std::string & );
		void setOverwritten( const std::string & );
};

#endif
