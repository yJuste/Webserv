// ************************************************************************** //
//                                                                            //
//                main.cpp                                                    //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"

// meme ip:port, si server_name vaut : host, alors on le choisit.
// si aucun correspond, c'est le premier declare.

int	main( int argc, char ** argv )
{
	std::vector<Server *> servers;
	try
	{
		if (argc != 2)
			throw FailedMainParameter();
		create_unique_program();
		std::srand(std::time(NULL));
		Print::newPalette();

		servers = configure_file(argv[1], servers);
		for (size_t i = 0; i < servers.size(); ++i)
			servers[i]->myConfig();
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		for (size_t i = 0; i < servers.size(); ++i)
			delete servers[i];
		servers.clear();
		return 1;
	}
	try
	{
		Supervisor supervisor;

		supervisor.hold(servers);
		supervisor.execution();
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return 2;
	}
	return 0;
}

/*
 *	By default, the program is running with the port '62034'.
 */

void	create_unique_program( void )
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		throw FailedSocket();

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(62034);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (bind(sock, (sockaddr *)&addr, sizeof(addr)) == -1)
		throw NotUniqueProcessus();
}
