// ************************************************************************** //
//                                                                            //
//                main.cpp                                                    //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"

volatile sig_atomic_t		g_stop = 0;
extern "C" void			on_stop( int ) { g_stop = 1; }

int	main( int argc, char ** argv )
{
	int unique_socket = -1;
	std::vector<Server *> servers;
	try
	{
		if (argc != 2)
			throw FailedMainParameter();
		unique_socket = create_unique_program();
		std::srand(std::time(NULL));
		Print::newPalette();

		servers = configure_file(argv[1], servers);
		for (size_t i = 0; i < servers.size(); ++i)
			servers[i]->myConfig();
	}
	catch (const std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		for (size_t i = 0; i < servers.size(); ++i)
			delete servers[i];
		servers.clear();
		if (unique_socket != -1)
			close(unique_socket);
		return 1;
	}
	try
	{
		signal(SIGINT, on_stop);
		signal(SIGTERM, on_stop);
		Supervisor supervisor;

		supervisor.hold(servers);
		supervisor.execution();
	}
	catch (const std::exception & e)
	{
		if (dynamic_cast<const std::bad_alloc *>(&e))
		{
			std::cerr << "     | " << std::string(APPLE_GREEN) << "Supervisor" << std::string(RESET) << ": Allocation Failed" << std::endl;
			return 2;
		}
		std::cerr << e.what() << std::endl;
		if (unique_socket != -1)
			close(unique_socket);
		return 3;
	}
	if (unique_socket != -1)
		close(unique_socket);
	return 0;
}

/*
 *	By default, the program is running with the port '62034'.
 */

int	create_unique_program( void )
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		throw FailedSocket();

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(62034);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (bind(sock, (sockaddr *)&addr, sizeof(addr)) == -1)
	{
		close(sock);
		throw NotUniqueProcessus();
	}
	return sock;
}
