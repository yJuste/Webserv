// ************************************************************************** //
//                                                                            //
//                main.cpp                                                    //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

#include "main.hpp"

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

int	main(int argc, char **argv)
{
	try
	{
		if (argc != 2)
			throw FailedMainParameter();
		create_unique_program();
		std::vector<Server *>	servers;
		SessionManager sessionManager;
		Poller poller;

		(void)argc;
		servers = configure_file(argv[1]);
		for ( size_t i = 0; i < servers.size(); ++i )
		{
			servers[i]->startup();
			poller.addFd(servers[i]->getSocket(), POLLIN, NULL);
			servers[i]->myConfig();
		}
		std::cout << "Server running... press Ctrl+C to stop.\n";
		poller.run(servers, sessionManager);
	}
	catch (const std::runtime_error& e) { std::cerr << e.what() << std::endl; }
	catch (std::exception & e) { std::cerr << e.what() << std::endl; }
	return 0;
}


