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

Location::Location() : _path(""), _redirect(false), _autoindex(false), _default(""), _upload("")
{
	_duplicate["methods"] = false;

	_overwritten["redirect"] = false;
	_overwritten["default"] = false;
	_overwritten["autoindex"] = false;
	_overwritten["upload"] = false;
}
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
bool	Location::getDuplicate( const std::string & parameter ) const { std::map<std::string, bool>::const_iterator it = _duplicate.find(parameter); return it != _duplicate.end() && it->second; }
bool	Location::getOverwritten( const std::string & parameter ) const { std::map<std::string, bool>::const_iterator it = _overwritten.find(parameter); return it != _overwritten.end() && it->second; }

// Setter

void	Location::setPath( const std::string & path ) { _path = path; }
void	Location::setMethods( const std::vector<std::string> & methods ) { _methods = methods; }
void	Location::setRedirect( bool redirect ) { _redirect = redirect; }
void	Location::setDefault( const std::string & def ) { _default = def; }
void	Location::setAutoindex( bool autoindex ) { _autoindex = autoindex; }
void	Location::addCgi( const std::string & extension, const std::string & program ) { _cgi[extension] = program; }
void	Location::setUpload( const std::string & upload ) { _upload = upload; }
void	Location::setDuplicate( const std::string & parameter ) { _duplicate[parameter] = true; }
void	Location::setOverwritten( const std::string & parameter ) { _overwritten[parameter] = true; }
