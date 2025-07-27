// ************************************************************************** //
//                                                                            //
//                debug.cpp                                                   //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"

// str_to_ascii
void stoa(const std::string& path)
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
