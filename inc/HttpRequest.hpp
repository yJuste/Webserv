/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 19:36:32 by layang            #+#    #+#             */
/*   Updated: 2025/09/02 20:41:10 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

// Standard Libraries

# include <string>
# include <map>

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

		bool _unchunked;
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
		bool isComplete();
		std::string unchunkBody(const std::string &raw);
};
	
#endif