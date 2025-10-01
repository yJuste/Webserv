// ************************************************************************** //
//                                                                            //
//                SessionManager.cpp                                          //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Layang                                        //
//                                                                            //
// ************************************************************************** //

# include "SessionManager.hpp"

SessionManager::SessionManager() { std::srand(std::time(NULL)); }
SessionManager::~SessionManager() {}

SessionManager::SessionManager( const SessionManager & s ) { *this = s; }

SessionManager & SessionManager::operator = ( const SessionManager & s )
{
	if (this != &s)
		_sessions = s._sessions;
	return *this;
}

// Methods

std::string	SessionManager::create( void )
{
	std::string sessionId;
	do { sessionId = _generateSessionId(); }
	while (_sessions.find(sessionId) != _sessions.end());

	Session session;
	session.expire = std::time(NULL) + 3600;
	session.bg_color = "#ffffff";
	session.counter = 1;
	_sessions[sessionId] = session;
	return sessionId;  
}

void	SessionManager::hasExpired( time_t now )
{
	while (!_expirations.empty())
	{
		std::multimap<time_t, std::string>::iterator it = _expirations.begin();
		if (it->first > now)
			break;
		const std::string & sid = it->second;
		_sessions.erase(sid);
		_expirations.erase(it);
        }
}

std::string	SessionManager::getSessionIdFromCookie( const std::string & cookie )
{
	size_t pos = cookie.find("session_id=");
	if (pos == std::string::npos)
		return "";
	pos += 11;
	size_t end = cookie.find(';', pos);
	if (end == std::string::npos)
		return cookie.substr(pos);
	else
		return cookie.substr(pos, end - pos);
}

// Private Method

std::string	SessionManager::_generateSessionId( void )
{
	std::ostringstream oss;
	for (int i = 0; i < 16; i++)
	{
		int r = std::rand() % 256;
		if (r < 16)
			oss << '0';
		oss << std::hex << r;
	}
	return oss.str();
}

// Getter

Session	* SessionManager::getSession( const std::string & sessionId )
{
	std::map<std::string, Session>::iterator it = _sessions.find(sessionId);
	if (it != _sessions.end())
		return &it->second;
	return NULL;
}
