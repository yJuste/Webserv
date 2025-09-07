// ************************************************************************** //
//                                                                            //
//                utils.cpp                                                   //
//                Created on  : xxx Sep xx xx:xx:xx 2025                      //
//                Last update : xxx Sep xx xx:xx:xx 2025                      //
//                Made by     : Layang                                        //
//                                                                            //
// ************************************************************************** //

# include "utils.hpp"

std::string	readFile( const std::string & path )
{
	std::ifstream file(path.c_str(), std::ifstream::binary);
	if (!file)
		return "";
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}
