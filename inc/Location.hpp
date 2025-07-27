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
# include <vector>
# include <map>

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

		const std::string & getPath() const;
		const std::vector<std::string> & getMethods() const;
		bool getRedirect() const;
		bool getAutoindex() const;
		const std::string & getDefault() const;
		const std::map<std::string, std::string> & getCgi() const;
		const std::string & getUpload() const;

		const std::string & getMethodX( int ) const;
		const std::string & getCgiX( const std::string & ) const;

		class Exception;
};

class	Location::Exception : public std::exception
{
	public : const char * what() const throw() { return "\033[31merror\033[0m: Location Error"; }
};

#endif
