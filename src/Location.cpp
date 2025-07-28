// ************************************************************************** //
//                                                                            //
//                Location.cpp                                                //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

# include "Location.hpp"
# include "Exceptions.hpp"

Location::Location() : _path(""), _redirect(false), _autoindex(false), _default(""), _upload("") {}
Location::~Location() {}

Location::Location( const Location & l ) { *this = l; }

Location	&Location::operator = ( const Location & l )
{
	if (this != &l)
	{
		_path = l.getPath();
		_methods = l.getMethods();
		_redirect = l.getRedirect();
		_autoindex = l.getAutoindex();
		_default = l.getDefault();
		_cgi = l.getCgi();
		_upload = l.getUpload();
	}
	return *this;
}

// Getter

const std::string	&Location::getPath() const { return _path; }
const std::vector<std::string>	&Location::getMethods() const { return _methods; }
bool	Location::getRedirect() const { return _redirect; }
bool	Location::getAutoindex() const { return _autoindex; }
const std::string	&Location::getDefault() const { return _default; }
const std::map<std::string, std::string>	&Location::getCgi() const { return _cgi; }
const std::string	&Location::getUpload() const { return _upload; }

const std::string	&Location::getMethodX( int idx ) const
{
	if (idx < 0 || idx >= static_cast<int>(_methods.size()))
		throw std::out_of_range("Invalid index: Server::getNames()");

	std::vector<std::string>::const_iterator	it = _methods.begin();
	std::advance(it, idx);
	return *it;
}

const std::string	&Location::getCgiX( const std::string & s ) const
{
	std::map<std::string, std::string>::const_iterator it = _cgi.find(s);
	if (it == _cgi.end())
		throw std::out_of_range("Invalid index: Server::getErrorPages()");
	return it->second;
}

// Setter

void	Location::setPath( const std::string & path ) { _path = path; }
void	Location::setMethods( const std::vector<std::string> & methods ) { _methods = methods; }
void	Location::setRedirect( bool redirect ) { _redirect = redirect; }
void	Location::setDefault( const std::string & def ) { _default = def; }
void	Location::setAutoindex( bool autoindex ) { _autoindex = autoindex; }
