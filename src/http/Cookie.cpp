// ************************************************************************** //
//                                                                            //
//                Cookie.cpp                                                  //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Cookie.hpp"

Cookie::Cookie() : _date("") { _cookie["id"] = std::rand(); std::cout << _cookie["id"] << std::endl; }
Cookie::~Cookie() {}

Cookie::Cookie( const Cookie & c ) { *this = c; }

Cookie & Cookie::operator = ( const Cookie & c )
{
	if (this != &c)
	{
		_cookie = c.getCookie();
		_date = c.getDate();
	}
	return *this;
}

// Getters

const std::map<std::string, std::string> & Cookie::getCookie() const { return _cookie; }
const std::string & Cookie::getDate() const { return _date; }
