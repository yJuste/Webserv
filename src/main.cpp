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
		for ( size_t i = 0; i < servers.size(); ++i )
		{
			servers[i].startup();
			servers[i].myConfig();
			servers[i].shutdown();
		}
	}
	catch (std::exception & e) { std::cerr << e.what() << std::endl; }
	return 0;
}
