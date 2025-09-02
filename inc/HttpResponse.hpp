/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 19:34:52 by layang            #+#    #+#             */
/*   Updated: 2025/09/02 12:07:12 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "Location.hpp"
# include "HttpRequest.hpp"
# include "Server.hpp"

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
	std::string toString() const;
	void handlePost(const HttpRequest &req, const Location *loc);
	void buildResponse(HttpRequest &req, const Server* server);
	
	// execute CGI，build HttpResponse
    //void executeCGI(const HttpRequest &req, const Location &loc);
};

#endif