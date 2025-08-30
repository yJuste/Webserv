// ************************************************************************** //
//                                                                            //
//                multi_ports.cpp                                             //
//                Created on  : xxx Aug xx xx:xx:xx 2025                      //
//                Last update : xxx Aug xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"

std::vector<Server *>	multi_ports( std::vector<Server *> servers )
{
	std::vector<Server *> res = servers;
	for (size_t i = 0; i < servers.size(); ++i)
	{
		std::vector<int> ports = servers[i]->getPort();

		for (size_t j = 1; j < ports.size(); ++j)
		{
			Server * server = new Server(*servers[i]);
			server->getPort().clear();
			server->addPort(ports[j]);
			res.push_back(server);
		}
		servers[i]->getPort().clear();
		servers[i]->addPort(ports[0]);
	}
	return res;
}
