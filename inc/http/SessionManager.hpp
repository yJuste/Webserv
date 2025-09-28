// ************************************************************************** //
//                                                                            //
//                SessionManager.hpp                                          //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Layang                                        //
//                                                                            //
// ************************************************************************** //

#ifndef SESSIONMANAGER_HPP
# define SESSIONMANAGER_HPP

// Standard Libraries

# include <string>
# include <map>
# include <ctime>
# include <sstream>
# include <iomanip>
# include <cstdlib>

// Dependences

struct Session
{
	std::string	username;
	std::string	mode;
	time_t		expire;
};

/*	HELP
 *
 *
 */

// ************************************************************************** //
//                                SessionManager Class                        //
// ************************************************************************** //

class	SessionManager
{
	private:

		std::map<std::string, Session>	_sessions;

		// Method

		std::string _generateSessionId();

	public:

		SessionManager();
		~SessionManager();

		SessionManager( const SessionManager & );
		SessionManager & operator = ( const SessionManager & );

		// Methods

		std::string createSession( const std::string & );
		void hasExpired();

		// Getter

		Session * getSession( const std::string & );
};

#endif
