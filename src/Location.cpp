// ************************************************************************** //
//                                                                            //
//                Location.cpp                                                //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Location.hpp"
# include "Exceptions.hpp"

Location::Location() : _path(""), _autoindex(false), _upload(""), _root("")
{
	for (int i = 0; g_methods[i]; ++i)
		_methods.push_back(g_methods[i]);

	_index.push_back("index.html");

	_overwritten["methods"] = false;
	_overwritten["allow_methods"] = false;
	_overwritten["allowed_methods"] = false;
	_overwritten["root"] = false;
	_overwritten["return"] = false;
	_overwritten["redirect"] = false;
	_overwritten["index"] = false;
	_overwritten["autoindex"] = false;
	_overwritten["upload"] = false;
	_overwritten["upload_store"] = false;
	_overwritten["cgi"] = false;
	_overwritten["cgi_ext"] = false;
	_overwritten["cgi_path"] = false;
}
Location::~Location() {}

Location::Location( const Location & l ) { *this = l; }

Location	&Location::operator = ( const Location & l )
{
	if (this != &l)
	{
		_path = l.getPath();
		_methods = l.getMethods();
		_return = l.getReturn();
		_autoindex = l.getAutoindex();
		_index = l.getIndex();
		_cgi = l.getCgi();
		_cgi_paths = l.getCgiPaths();
		_upload = l.getUpload();
		_root = l.getRoot();

		_overwritten = l.getOverwritten();
	}
	return *this;
}

// Getters

const std::string & Location::getPath() const { return _path; }
const std::vector<std::string> & Location::getMethods() const { return _methods; }
std::vector<std::string> & Location::getMethods() { return _methods; }
const std::map<int, std::string> & Location::getReturn() const { return _return; }
bool Location::getAutoindex() const { return _autoindex; }
const std::vector<std::string> & Location::getIndex() const { return _index; }
std::vector<std::string> & Location::getIndex() { return _index; }
const std::map<std::string, std::string> & Location::getCgi() const { return _cgi; }
const std::vector<std::string> & Location::getCgiPaths() const { return _cgi_paths; }
const std::string & Location::getUpload() const { return _upload; }
std::string & Location::getUpload() { return _upload; }
const std::string & Location::getRoot() const { return _root; }
std::string & Location::getRoot() { return _root; }

const std::map<std::string, bool> & Location::getOverwritten() const { return _overwritten; }
bool Location::getOverwrittenX( const std::string & parameter ) const { std::map<std::string, bool>::const_iterator it = _overwritten.find(parameter); return it != _overwritten.end() && it->second; }

// Setters

void Location::setPath( const std::string & path ) { _path = path; }
void Location::setMethods( const std::vector<std::string> & methods ) { _methods = methods; }
void Location::setReturn( int n, const std::string & ret ) { _return[n] = ret; }
void Location::setIndex( const std::vector<std::string> & index ) { _index = index; }
void Location::addIndex( const std::string & index ) { _index.push_back(index); }
void Location::setAutoindex( bool autoindex ) { _autoindex = autoindex; }
void Location::setCgi( const std::map<std::string, std::string> & cgi ) { _cgi = cgi; }
void Location::addCgi( const std::string & extension, const std::string & program ) { _cgi[extension] = program; }
void Location::addCgiExt( const std::string & extension ) { std::map<std::string, std::string>::iterator it = _cgi.find(extension); if (it == _cgi.end() || it->second.empty()) _cgi[extension] = ""; }
void Location::addCgiPath( const std::string & path ) { _cgi_paths.push_back(path); }
void Location::setUpload( const std::string & upload ) { _upload = upload; }
void Location::setRoot( const std::string & root ) { _root = root; }

void Location::setOverwritten( const std::string & parameter ) { _overwritten[parameter] = true; }
