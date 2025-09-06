/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 19:34:52 by layang            #+#    #+#             */
/*   Updated: 2025/09/06 17:06:39 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

// Dependences

# include "Location.hpp"
# include "HttpRequest.hpp"
# include "Server.hpp"
#include "file_utils.hpp"

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

class HttpResponse {
private:
	int _status;
	std::string _statusText;
	std::map<std::string, std::string> _headers;
	std::string _body;
	// inner check functions
	const Location* findLocation(const std::string &path, const Server* server) const;
	bool allowsMethod(const Location* loc, const std::string &method) const;
	std::string buildHeaders() const;

	public:
	HttpResponse();
	~HttpResponse();

	// set status and body
	void setStatus(int status, const std::string &text);
	void setBody(const std::string &body);
	void setHeader(const std::string &key, const std::string &value);

	// build full HTTP response
	void executeCGI(const HttpRequest &req,
                              	const Location &loc,
                              	const std::string &filePath,
                              	const Server &server);
	std::string toString(const HttpRequest &req) const;
	void handlePost(const HttpRequest &req, const Location *loc);
	void buildResponse(HttpRequest &req, const Server* server);

};

#endif