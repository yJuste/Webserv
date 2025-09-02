/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 09:02:10 by layang            #+#    #+#             */
/*   Updated: 2025/09/01 23:41:36 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
# define CONNECTION_HPP

// Standard Libraries
# include <string>
# include <map>
# include <vector>
# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <errno.h>
// ************************************************************************** //
//                                  Dependences                               //
// ************************************************************************** //

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "main.hpp"     // for READ_SIZE
# include "Server.hpp"


// ************************************************************************** //
//                                  Prototypes                                //
// ************************************************************************** //

class Connection {
	private:
		int	_fd;
		//std::vector<Location> _locations;
		Server* _server;
		std::string _readBuffer;
		std::string _writebBuffer;
		HttpRequest _request;
	public:
		Connection(int fd, Server* server);
		// Member functions
		bool hasDataToWrite() const;
		int getFd() const;
		bool readFromClient();
		bool writeToClient();
};
	
#endif