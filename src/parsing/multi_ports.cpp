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

std::vector<Server *>	multi_ports( std::vector<Server *> & servers )
{
	std::vector<Server *> res = servers;
	for (size_t i = 0; i < servers.size(); ++i)
	{
		std::vector<int> ports = servers[i]->getEveryPort();
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

	std::vector<Server *>::iterator it = res.begin();
	for (; it != res.end(); ++it)
		for (std::vector<Server *>::iterator cit = it + 1; cit != res.end(); ++cit)
			if ((*it)->getHost() == (*cit)->getHost() && (*it)->getPort() == (*cit)->getPort())
				(*cit)->setDefault(false);
	for (it = res.begin(); it != res.end();)
	{
		if ((*it)->getDefault() == false)
			it = res.erase(it);
		else
			++it;
	}
	return res;
}
