// ************************************************************************** //
//                                                                            //
//                Location.hpp                                                //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

#ifndef LOCATION_HPP
# define LOCATION_HPP

// Standard Libraries

# include <iostream>
# include <map>
# include <list>

// ************************************************************************** //
//                                Location Class                              //
// ************************************************************************** //

class	Location
{
	private:
			std::string				_path;
			std::vector<std::string>		_methods;
			bool					_redirect;
			bool					_autoindex;
			std::string				_default;
			std::map<std::string, std::string>	_cgi;
			std::string				_upload;

	public:

		Location();
		~Location();

		Location( const Location & );
		Location & operator = ( const Location & );

		// ~etter

		std::string getPath() const;
		std::vector<std::string> getMethods() const;
		bool getRedirect() const;
		bool getAutoindex() const;
		std::string getDefault() const;
		std::map<std::string, std::string> getCgi() const;
		std::string getUpload() const;

		std::string getMethodX( int ) const;
		std::string getCgiX( std::string & ) const;

		class Exception;
};

class	Location::Exception : public std::exception
{
	public : const char * what() const throw() { return "\033[31merror\033[0m: Location Error"; }
};

#endif
