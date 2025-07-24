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

		servers = configFile(argv[1]);
		std::cout << "Port: " << servers[0].getPort() << std::endl;
		std::cout << "Host: " << servers[0].getHost() << std::endl;
	}
	catch (std::exception & e) { std::cout << e.what() << std::endl; }
	return 0;
}
