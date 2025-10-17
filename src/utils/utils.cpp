// ************************************************************************** //
//                                                                            //
//                utils.cpp                                                   //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "utils.hpp"

std::string	rounded( size_t bytes )
{
	const char * units[] = {"bytes", "KB", "MB", "GB", "TB"};
	double size = static_cast<double>(bytes);
	int unit = 0;
	while ( size >= 1024.0 && unit < 4 )
	{
		size /= 1024.0;
		++unit;
	}

	std::stringstream ss;
	ss.precision(2);
	ss.setf(std::ios::fixed);
	ss << size << " " << units[unit];
	return ss.str();
}

std::string	getExtension( const std::string & file )
{
	std::string ext;
	size_t dot = file.rfind('.');
	if (dot != std::string::npos)
		ext = file.substr(dot);
	return ext;
}

std::string	to_clean( const std::string & root, const std::string & fullPath )
{
	std::string cleanRoot = root;
	std::string cleanPath = fullPath;
	if (cleanRoot.substr(0, 2) == "./")
		cleanRoot = cleanRoot.substr(2);
	if (cleanPath.substr(0, 2) == "./")
		cleanPath = cleanPath.substr(2);
	if (!cleanRoot.empty() && cleanRoot[cleanRoot.size() - 1] != '/')
		cleanRoot += "/";
	if (cleanPath.substr(0, cleanRoot.size()) == cleanRoot)
	{
		std::string stripped = cleanPath.substr(cleanRoot.size());
		if (stripped.empty())
			return "./";
		return stripped;
	}
	return cleanPath;
}

void	print_status_cgi( const std::string & response, const std::string & color, int socket )
{
	std::string line;
	std::istringstream iss(response);
	std::getline(iss, line);
	std::istringstream words(line);
	std::string word1, word2;
	words >> word1 >> word2;
	std::string rest;
	std::getline(words, rest);
	rest.erase(std::remove(rest.begin(), rest.end(), '\r'), rest.end());
	rest.erase(std::remove(rest.begin(), rest.end(), '\n'), rest.end());
	size_t start = 0;
	while (start < rest.size() && (rest[start] == ' ' || rest[start] == '\t')) ++start;
	rest = rest.substr(start);
	std::string statusLine = "[" + std::string(APPLE_GREEN);
	statusLine += word2;
	if (!rest.empty())
		statusLine += " " + rest;
	statusLine += std::string(RESET) + "]";
	Print::debug(color, socket, "Response :");
	Print::enval(color, "     | Status", RESET, statusLine);
}

bool	setBadGateway( std::string & wbuf, ssize_t & original, const std::string & reason )
{
	std::ostringstream oss;
	oss << "HTTP/1.1 502 Bad Gateway\r\nContent-Type: text/html\r\n\r\n<html><body><h1>502 Bad Gateway</h1><p>" << reason << "</p></body></html>";
	wbuf = oss.str();
	original = static_cast<ssize_t>(wbuf.size());
	return true;
}

static std::string	trim_cr_lf( const std::string & s )
{
	std::string r = s;
	while (!r.empty() && (r[r.size() - 1] == '\r' || r[r.size() - 1] == '\n'))
		r.erase(r.size() - 1);
	return r;
}

static std::string	trim_left( const std::string & s )
{
	size_t i = 0;
	while (i < s.size() && (s[i] == ' ' || s[i] == '\t'))
		++i;
	return s.substr(i);
}

static std::string	trim_right( const std::string & s )
{
    if (s.empty()) return s;
    size_t i = s.size();
    while (i > 0 && (s[i-1] == ' ' || s[i-1] == '\t')) --i;
    return s.substr(0, i);
}

static bool	is_valid_header_name( const std::string & name )
{
	if (name.empty())
		return false;
	for (size_t i = 0; i < name.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(name[i]);
		if (!(std::isalnum(c) || name[i] == '-')) return false;
	}
	return true;
}

static std::map<std::string, std::vector<std::string> >	parse_headers_at( const std::string & text, size_t start, size_t & nextPos )
{
	std::map<std::string, std::vector<std::string> > headers;
	size_t pos = start;
	if (pos >= text.size())
	{
		nextPos = pos;
		return headers;
	}
	size_t nl0 = text.find('\n', pos);
	size_t lineEnd0 = (nl0 == std::string::npos) ? text.size() : nl0 + 1;
	std::string firstLine = text.substr(pos, lineEnd0 - pos);
	std::string firstTrim = trim_cr_lf(firstLine);
	size_t colon0 = firstTrim.find(':');
	if (colon0 == std::string::npos)
	{
		nextPos = start;
		return headers;
	}
	std::string candidateName = trim_right(firstTrim.substr(0, colon0));
	if (!is_valid_header_name(candidateName))
	{
		nextPos = start;
		return headers;
	}
	for (;;)
	{
		if (pos >= text.size())
		{
			nextPos = pos;
			return headers;
		}
		size_t nl = text.find('\n', pos);
		size_t lineEnd = (nl == std::string::npos) ? text.size() : nl + 1;
		std::string line = text.substr(pos, lineEnd - pos);
		std::string l = trim_cr_lf(line);
		if (l.empty())
		{
			nextPos = lineEnd;
			return headers;
		}
		size_t colon = l.find(':');
		if (colon == std::string::npos)
		{
			nextPos = start;
			return std::map<std::string, std::vector<std::string> >();
		}
		std::string key = trim_right(l.substr(0, colon));
		if (!is_valid_header_name(key))
		{
			nextPos = start;
			return std::map<std::string, std::vector<std::string> >();
		}
		std::string val = l.substr(colon + 1);
		val = trim_left(val);
		val = trim_right(val);
		headers[key].push_back(val);
		if (nl == std::string::npos)
		{
			nextPos = text.size();
			return headers;
		}
		pos = lineEnd;
	}
}

static void	merge_cgi_overrides( std::map<std::string, std::vector<std::string> > &base, const std::map<std::string, std::vector<std::string> > & cgi )
{
	for (std::map<std::string, std::vector<std::string> >::const_iterator it = cgi.begin(); it != cgi.end(); ++it)
	{
		const std::string & k = it->first;
		const std::vector<std::string> & vals = it->second;
		if (k == "Set-Cookie")
			for (size_t i = 0; i < vals.size(); ++i)
				base[k].push_back(vals[i]);
		else
		{
			base[k].clear();
			for (size_t i = 0; i < vals.size(); ++i)
			base[k].push_back(vals[i]);
		}
	}
}

void	finish_cgi( std::string & buf )
{
	size_t first_sep = buf.find("\r\n\r\n");
	std::string initialBlock;
	std::string rest;
	if (first_sep == std::string::npos)
	{
		initialBlock = "";
		rest = buf;
	}
	else
	{
		initialBlock = buf.substr(0, first_sep);
		rest = buf.substr(first_sep + 4);
	}
	size_t consumedInit = 0;
	std::map<std::string, std::vector<std::string> > initialHeaders;
	if (!initialBlock.empty())
		initialHeaders = parse_headers_at(initialBlock, 0, consumedInit);
	bool initialHasStatusLine = false;
	std::string initialStatusLine;
	if (!initialBlock.empty())
	{
		size_t posnl = initialBlock.find("\r\n");
		std::string firstLine = (posnl == std::string::npos) ? initialBlock : initialBlock.substr(0, posnl);
		firstLine = trim_cr_lf(firstLine);
		if (firstLine.size() >= 5 && firstLine.substr(0,5) == "HTTP/")
		{
			initialHasStatusLine = true;
			initialStatusLine = firstLine;
			if (initialHeaders.empty())
			{
				size_t startAfter = (posnl == std::string::npos) ? initialBlock.size() : posnl + 2;
				size_t nxt = 0;
				std::map<std::string, std::vector<std::string> > hdrs = parse_headers_at(initialBlock, startAfter, nxt);
				if (!hdrs.empty())
					initialHeaders = hdrs;
			}
		}
	}
	size_t consumedCgi = 0;
	std::map<std::string, std::vector<std::string> > cgiHeaders = parse_headers_at(rest, 0, consumedCgi);
	std::string body = rest.substr(consumedCgi);
	std::string cgiStatusLine;
	if (cgiHeaders.find("Status") != cgiHeaders.end() && !cgiHeaders["Status"].empty())
	{
		cgiStatusLine = std::string("HTTP/1.1 ") + cgiHeaders["Status"][0];
		cgiHeaders.erase("Status");
	}
	if (initialHasStatusLine)
	{
		merge_cgi_overrides(initialHeaders, cgiHeaders);
		std::ostringstream out;
		out << initialStatusLine << "\r\n";
		for (std::map<std::string, std::vector<std::string> >::iterator it = initialHeaders.begin(); it != initialHeaders.end(); ++it)
		{
			const std::string &key = it->first;
			if (key == "Content-Length")
				continue;
			const std::vector<std::string> &vals = it->second;
			if (key == "Set-Cookie")
				for (size_t i = 0; i < vals.size(); ++i)
					out << key << ": " << vals[i] << "\r\n";
			else
				if (!vals.empty())
					out << key << ": " << vals[0] << "\r\n";
		}
		out << "Content-Length: " << body.size() << "\r\n";
		out << "\r\n" << body;
		buf = out.str();
		return;
	}
	merge_cgi_overrides(initialHeaders, cgiHeaders);
	std::string statusLine = cgiStatusLine.empty() ? std::string("HTTP/1.1 200 OK") : cgiStatusLine;
	std::ostringstream out;
	out << statusLine << "\r\n";
	for (std::map<std::string, std::vector<std::string> >::iterator it = initialHeaders.begin(); it != initialHeaders.end(); ++it)
	{
		const std::string & key = it->first;
		if (key == "Content-Length")
			continue;
		const std::vector<std::string> & vals = it->second;
		if (key == "Set-Cookie")
			for (size_t i = 0; i < vals.size(); ++i)
				out << key << ": " << vals[i] << "\r\n";
		else
			if (!vals.empty())
				out << key << ": " << vals[0] << "\r\n";
	}
	out << "Content-Length: " << body.size() << "\r\n";
	out << "\r\n" << body;
	buf = out.str();
}
