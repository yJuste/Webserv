// ************************************************************************** //
//                                                                            //
//                Cookie.hpp                                                  //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

#ifndef COOKIE_HPP
# define COOKIE_HPP

// Standard Libraries

# include <iostream>
# include <map>

/*	HELP
 *
 */

// ************************************************************************** //
//                                 Cookie Class                               //
// ************************************************************************** //

class	Cookie
{
	private:

		std::map<std::string, std::string>	_cookie;
		std::string				_date;

	public:

		Cookie();
		~Cookie();

		Cookie( const Cookie & );
		Cookie & operator = ( const Cookie & );

		const std::map<std::string, std::string> & getCookie() const;
		const std::string & getDate() const;
};

#endif
