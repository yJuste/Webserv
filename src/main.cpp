// ************************************************************************** //
//                                                                            //
//                main.cpp                                                    //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"

int	main(int argc, char **argv)
{
	(void)argc;
	try
	{
		std::vector<Server>	servers;

		servers = configure_file(argv[1]);
		std::cout << "Host: " << servers[0].getHost() << std::endl;
		std::cout << "Port: " << servers[0].getPort() << std::endl;
		std::cout << "Root: " << servers[0].getRoot() << std::endl;
		std::cout << "Default: " << servers[0].getDefault() << std::endl;
		std::vector<std::string>	names = servers[0].getNames();
		for ( std::vector<std::string>::iterator it = names.begin(); it != names.end(); ++it )
			std::cout << "Names: " << *it << std::endl;
		std::map<int, std::string>	errors = servers[0].getErrorPages();
		for ( std::map<int, std::string>::iterator it = errors.begin(); it != errors.end(); ++it )
    		std::cout << "Code: " << it->first << ", Path: " << it->second << std::endl;
	}
	catch (std::exception & e) { std::cout << e.what() << std::endl; }
	return 0;
}
