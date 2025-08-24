// ************************************************************************** //
//                                                                            //
//                utils.cpp                                                   //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"

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
void	stoa( const std::string& path )
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
