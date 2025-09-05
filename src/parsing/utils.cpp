// ************************************************************************** //
//                                                                            //
//                utils.cpp                                                   //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
#include <sys/stat.h>   // stat, S_ISREG, S_ISDIR
#include <dirent.h>     // DIR*, opendir, closedir
#include <unistd.h>     // access
#include <iostream>
#include <iomanip>      // std::setw

// handle file/folder accessibilty. (access + stat)
int	acstat( const char * path, int mode )
{
	struct stat	buf;

	if (stat(path, &buf) == -1)
		return -1;
	if (access(path, mode) == -1)
		return -1;
	if (S_ISREG(buf.st_mode))
		return 1;
	if (S_ISDIR(buf.st_mode))
	{
		DIR *dir = opendir(path);
		if (!dir)
			return -1;
		closedir(dir);
		return 2;
	}
	return -1;
}

// DEBUG FUNCTION: str_to_ascii
void	stoa( const std::string & path )
{
	std::cout << std::endl;
	std::cout << "\033[91mDEBUG MODE:\033[0m stoa(): '\033[93m" << path << "\033[0m'.\n";

	for (size_t i = 0; i < path.size(); ++i)
	{
		std::cout << std::setw(10) << "[\033[93m" << std::setw(2) << i << "\033[0m]"
		<< " = '\033[91m" << path[i] << "\033[0m'"
		<< " (" << std::setw(3) << static_cast<int>(path[i]) << ")   ";

		if ((i + 1) % 3 == 0)
			std::cout << "\n";
	}
	std::cout << std::endl << std::endl;
}

// Regarde si le chemin est relatif et si oui, normalise.
int	relative( const std::string & path )
{
	int	i = 0;

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

// Test un dossier et le normalise.
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
