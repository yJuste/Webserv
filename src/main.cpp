// ************************************************************************** //
//                                                                            //
//                main.cpp                                                    //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"

// Si la longeur stipulé sur le message est trop grande par rapport au message, ca bloque attention.

int	main(int argc, char **argv)
{
	try
	{
		if (argc != 2)
			throw FailedMainParameter();
		create_unique_program();
		std::vector<Server *> servers = configure_file(argv[1]);

		for (size_t i = 0; i < servers.size(); ++i)
			servers[i]->myConfig();

		Supervisor supervisor;

		supervisor.hold(servers);
		supervisor.execution();
	}
	catch (std::exception & e) { std::cerr << e.what() << std::endl; }
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
