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
	try
	{
		std::vector<Server>	servers;

		(void)argc;
		servers = configure_file(argv[1]);
		servers[0].startup();
		servers[0].myConfig();
		servers[0].shutdown();
		servers[1].startup();
		servers[1].myConfig();
		servers[1].shutdown();
	}
	catch (std::exception & e) { std::cerr << e.what() << std::endl; }
	return 0;
}
