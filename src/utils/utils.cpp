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

void	finish_cgi( std::string & buf )
{
	size_t sep = buf.find("\r\n\r\n");
	if (sep == std::string::npos)
		return;

	size_t bodySize = buf.size() - (sep + 4);
	const std::string headerLen = "Content-Length:";
	size_t posLen = buf.find(headerLen);
	if (posLen != std::string::npos)
	{
		size_t end = buf.find("\r\n", posLen);
		if (end != std::string::npos)
		{
			std::ostringstream oss;
			oss << headerLen << " " << bodySize;
			buf.replace(posLen, end - posLen, oss.str());
		}
	}
	else
	{
		std::ostringstream oss;
		oss << "Content-Length: " << bodySize << "\r\n";
		buf.insert(sep + 2, oss.str());
	}
	const std::string headerType = "Content-Type:";
	size_t posType = buf.find(headerType);
	if (posType != std::string::npos)
	{
		size_t end = buf.find("\r\n", posType);
		if (end != std::string::npos)
		{
			std::string newType = "Content-Type: text/html";
			buf.replace(posType, end - posType, newType);
		}
	}
	else
	{
		std::string newType = "Content-Type: text/html\r\n";
		buf.insert(sep + 2, newType);
	}
}
