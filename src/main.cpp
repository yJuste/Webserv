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
		std::cout << "Max Body Size: " << servers[0].getMaxSize() << std::endl;

		std::vector<Location>	locations = servers[0].getLocations();
		std::cout << "Location : " << locations[0].getPath() << std::endl;
		std::vector<std::string>	methods = locations[0].getMethods();
		for ( std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); ++it )
			std::cout << "Methods: " << *it << std::endl;
		std::cout << "Redirect: " << locations[0].getRedirect() << std::endl;
		std::cout << "Default: " << locations[0].getDefault() << std::endl;
		std::cout << "Autoindex: " << locations[0].getAutoindex() << std::endl;
	}
	catch (std::exception & e) { std::cout << e.what() << std::endl; }
	return 0;
}
