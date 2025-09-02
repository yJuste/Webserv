/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poller.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 08:18:57 by layang            #+#    #+#             */
/*   Updated: 2025/09/02 20:29:14 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLLER_HPP
# define POLLER_HPP

// Standard Libraries
# include <string>
# include <map>
# include <vector>
# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <errno.h>
# include <poll.h> 

// ************************************************************************** //
//                                  Dependences                               //
// ************************************************************************** //

# include "Connection.hpp"
# include "Server.hpp"

// ************************************************************************** //
//                                  Prototypes                                //
// ************************************************************************** //

class Poller {
	private:
		std::vector<pollfd>	_fds;
		std::map<int, Connection *> _connections;
	public:
		void addFd(int fd, short events, Connection *conn);
		void removeFd(int fd);
		void modifyFd(int fd, short events);
		void run(const std::vector<Server*>& servers);
};
	
#endif