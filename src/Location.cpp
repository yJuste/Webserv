// ************************************************************************** //
//                                                                            //
//                Location.cpp                                                //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Location.hpp"

Location::Location() : _path(""), _root(""), _upload(""), _autoindex(false)
{
	for (int i = 0; g_methods[i]; ++i)
		_methods.push_back(g_methods[i]);
	_overwritten["cgi"] = 0;
	_overwritten["root"] = 0;
	_overwritten["index"] = 0;
	_overwritten["upload"] = 0;
	_overwritten["methods"] = 0;
	_overwritten["redirect"] = 0;
	_overwritten["autoindex"] = 0;
}
Location::~Location() {}

Location::Location( const Location & l ) { *this = l; }

Location	&Location::operator = ( const Location & l )
{
	if (this != &l)
	{
		_path = l._path;
		_root = l._root;
		_upload = l._upload;
		_methods = l._methods;
		_return = l._return;
		_autoindex = l._autoindex;
		_index = l._index;
		_cgi = l._cgi;
		_cgi_paths = l._cgi_paths;
		_overwritten = l._overwritten;
	}
	return *this;
}

// Getters

const std::string & Location::getPath() const { return _path; }
const std::string & Location::getRoot() const { return _root; }
const std::string & Location::getUpload() const { return _upload; }
const std::vector<std::string> & Location::getMethods() const { return _methods; }
const std::map<int, std::string> & Location::getReturn() const { return _return; }
bool Location::getAutoindex() const { return _autoindex; }
const std::vector<std::string> & Location::getIndex() const { return _index; }
const std::map<std::string, std::string> & Location::getCgi() const { return _cgi; }
const std::vector<std::string> & Location::getCgiPaths() const { return _cgi_paths; }
const std::map<std::string, int> & Location::getOverwritten() const { return _overwritten; }

// Setters

void Location::setPath( const std::string & path ) { _path = path; }
void Location::setRoot( const std::string & root ) { _root = root; }
void Location::setUpload( const std::string & upload ) { _upload = upload; }
void Location::setMethods( const std::vector<std::string> & methods ) { _methods = methods; }
void Location::setReturn( int n, const std::string & ret ) { _return[n] = ret; }
void Location::setAutoindex( bool autoindex ) { _autoindex = autoindex; }
void Location::setIndex( const std::vector<std::string> & index ) { _index = index; }
void Location::setCgi( const std::map<std::string, std::string> & cgi ) { _cgi = cgi; }
void Location::addCgi( const std::string & extension, const std::string & program ) { _cgi[extension] = program; }
void Location::addCgiExt( const std::string & extension ) { _cgi[extension] = ""; }
void Location::addCgiPath( const std::string & path ) { _cgi_paths.push_back(path); }
void Location::setOverwritten( const std::string & parameter ) { _overwritten[parameter] += 1; }
