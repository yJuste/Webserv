/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 19:36:32 by layang            #+#    #+#             */
/*   Updated: 2025/09/05 11:22:06 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

// Standard Libraries

#include <string>
#include <map>
#include <sstream>
#include <cstdlib>
// ************************************************************************** //
//                                  Prototypes                                //
// ************************************************************************** //

class HttpRequest {
	private:
		std::string _method;
		std::string _path;
		std::string _httpVersion;

		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _headerPart;

		bool _unchunked;
		bool _printed;
	public:
		HttpRequest();
		~HttpRequest();
		// Member functions
		void parseRequest(const std::string &rawRequest); // Main Parsing Function
		void reset();
		void discardBody();
		std::string getMethod() const;
		std::string getPath() const;
		std::string getHttpVersion() const;
		std::string getRequestBody() const;
		std::string getHeader(const std::string &key) const;
		std::map<std::string, std::string> getHeaders() const;
		bool hasPrinted() const;
		void setPrinted(bool val);
		bool isComplete();
		std::string unchunkBody(const std::string &raw);
};
	
#endif