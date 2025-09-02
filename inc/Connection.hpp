/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 09:02:10 by layang            #+#    #+#             */
/*   Updated: 2025/09/02 20:35:45 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
# define CONNECTION_HPP

// Defines

# ifndef READ_SIZE
#  define READ_SIZE 4096
# endif

// Dependences

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
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