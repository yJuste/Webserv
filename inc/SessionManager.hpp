/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionManager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 11:28:30 by layang            #+#    #+#             */
/*   Updated: 2025/09/23 12:05:09 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSIONMANAGER_HPP
# define SESSIONMANAGER_HPP
# include <string>
# include <map>
# include <ctime>
# include <sstream>
# include <iomanip>
# include <cstdlib>

struct Session {
	std::string username;
	std::string mode;
	time_t      expire;
};

class SessionManager {
	private:
		std::map<std::string, Session> _sessions;
		std::string generateSesssionId();
	public:
		SessionManager();
		std::string createSession(const std::string username);
		Session* getSession(const std::string& sessionId);
		void cleanupExpired();
};

#endif