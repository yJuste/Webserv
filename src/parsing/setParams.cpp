// ************************************************************************** //
//                                                                            //
//                setParams.cpp                                               //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"

// -----------------------------------------------------------------------------
void	setListen( std::vector<std::string>::iterator & it, Server & server );
void	setRoot( std::vector<std::string>::iterator & it, Server & server );
// -----------------------------------------------------------------------------

void	setParameters( std::vector<std::string>::iterator & it, Server & server )
{
	if (*it == "listen")
		setListen(++it, server);
	if (*it == "root")
		setRoot(++it, server);
}

void	setListen( std::vector<std::string>::iterator & it, Server & server )
{
	std::string	str;

	str = *it;
	if (!str.empty() && str.back() == ';')
		str.pop_back();

	size_t		sep;

	sep = str.find(':');
	if (sep != std::string::npos)
	{
		std::string	host = str.substr(0, sep);
		std::string	port = str.substr(sep + 1);

		server.setHost(host);
		server.setPort(std::atoi(port.c_str()));
	}
	else
		server.setPort(std::atoi(str.c_str()));
}

void	setRoot( std::vector<std::string>::iterator & it, Server & server )
{
	std::string	str;

	str = *it;
	if (!str.empty() && str.back() == ';')
		str.pop_back();

	server.setRoot(str);
}
