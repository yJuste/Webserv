// ************************************************************************** //
//                                                                            //
//                utils.cpp                                                   //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "utils.hpp"
# include "Exceptions.hpp"

int	acstat( const char * path, int mode )
{
	struct stat buf;
	if (stat(path, &buf) == -1)
	{
		if (errno == EACCES)
			return -2;
		return -1;
	}
	if (access(path, mode) == -1)
	{
		if (errno == EACCES)
			return -2;
		return -1;
	}
	if (S_ISREG(buf.st_mode))
		return 1;
	if (S_ISDIR(buf.st_mode))
	{
		DIR *dir = opendir(path);
		if (!dir)
		{
			if (errno == EACCES)
				return -2;
			return -1;
		}
		closedir(dir);
		return 2;
	}
	return -1;
}

int	relative( const std::string & path )
{
	int i = 0;
	if (path[0] == '.')
	{
		if (path[++i] != '/')
			i = 0;
		else
			while (path[i] == '/')
				++i;
	}
	return i;
}

std::string	handle_folder( std::string s )
{
	if (!s.size())
		s += '.';
	if (s[s.size() - 1] != '/')
		s += '/';
	if (acstat(s.c_str(), F_OK | R_OK) != 2)
		throw FailedAcstat(s.c_str());
	return s;
}
