/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poller.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 08:18:57 by layang            #+#    #+#             */
/*   Updated: 2025/09/24 15:46:28 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLLER_HPP
# define POLLER_HPP

// Standard Libraries

# include <poll.h> 
# include <unistd.h>

// Dependences
# include "Server.hpp"
# include "Connection.hpp"
# include "SessionManager.hpp"
//class Connection;

// ************************************************************************** //
//                                  Prototypes                                //
// ************************************************************************** //

class Poller {
	private:
		std::vector<pollfd>	_fds;
		std::map<int, Connection *> _connections;
	public:
		~Poller();
		void addFd(int fd, short events, Connection *conn);
		void removeFd(int fd);
		void modifyFd(int fd, short events);
		void run(const std::vector<Server*>& servers, SessionManager& sessionManager);
};
	
#endif
