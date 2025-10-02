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
	time_t		expire;
	std::string	bg_color;
	size_t		counter;

	// Methods

	void incrementCounter() { counter++; }

	// Getters

	const std::string & getBgColor() const { return bg_color; }
	size_t getCounter() const { return counter; }

	// Setter

	void setBgColor( const std::string & color ) { bg_color = color; }
};

/*	HELP
 *
 *	Session supports background_color, counter.
 */

// ************************************************************************** //
//                                SessionManager Class                        //
// ************************************************************************** //

class	SessionManager
{
	private:

		std::map<std::string, Session>		_sessions;
		std::multimap<time_t, std::string>	_expirations;

		// Method

		std::string _generateSessionId();

	public:

		SessionManager();
		~SessionManager();

		SessionManager( const SessionManager & );
		SessionManager & operator = ( const SessionManager & );

		// Methods

		std::string create();
		void hasExpired( time_t );
		std::string getSessionIdFromCookie( const std::string & );

		// Getter

		Session * getSession( const std::string & );
};

#endif
