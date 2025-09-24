/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 09:02:10 by layang            #+#    #+#             */
/*   Updated: 2025/09/24 12:00:04 by layang           ###   ########.fr       */
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
# include "SessionManager.hpp"

// ************************************************************************** //
//                                  Prototypes                                //
// ************************************************************************** //

class Connection {
	private:
		int	_fd;
		Server* _server;
		SessionManager& _sessionManager;
		std::string _readBuffer;
		std::string _writebBuffer;
		HttpRequest _request;
	public:
		Connection(int fd, Server* server, SessionManager& sessionManager);
		~Connection();
		// Member functions
		bool hasDataToWrite() const;
		int getFd() const;
		void resetRequest();
		bool readFromClient();
		bool writeToClient();
};
	
#endif
