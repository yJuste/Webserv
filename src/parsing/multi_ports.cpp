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
			std::vector<int> singlePort;
			singlePort.push_back(ports[j]);
			server->setPort(singlePort);
			res.push_back(server);
		}
		std::vector<int> firstPort;
		firstPort.push_back(ports[0]);
		servers[i]->setPort(firstPort);
	}
	return res;
}
