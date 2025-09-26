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
