// ************************************************************************** //
//                                                                            //
//                init_server.cpp                                             //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

# include "main.hpp"
# include "Exceptions.hpp"

// -----------------------------------------------------------------------------
void	setListen( std::vector<std::string>::iterator & it, Server & server );
void	setHost( std::vector<std::string>::iterator & it, Server & server );
void	setRoot( std::vector<std::string>::iterator & it, Server & server );
// -----------------------------------------------------------------------------

void	init_server( std::vector<std::string> & words, std::vector<std::string>::iterator & it, Server & server )
{
	if (*it == "listen")
		setListen(++it, server);
	else if (*it == "host")
		setHost(++it, server);
	else if (*it == "root")
		setRoot(++it, server);
	else if (*it == "location")
		init_location(words, ++it, server);
	else
		throw InvalidParameter(it->c_str());
}

void	setListen( std::vector<std::string>::iterator & it, Server & server )
{
	std::string	str;

	str = *it;
	if (str.find_first_of(";") == std::string::npos)
		throw NoEndingSemicolon();
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

void	setHost( std::vector<std::string>::iterator & it, Server & server )
{
	std::string	str;

	str = *it;
	if (str.find_first_of(";") == std::string::npos)
		throw NoEndingSemicolon();
	str.pop_back();

	server.setHost(str);
}

void	setRoot( std::vector<std::string>::iterator & it, Server & server )
{
	std::string	str;

	str = *it;
	if (str.find_first_of(";") == std::string::npos)
		throw NoEndingSemicolon();
	str.pop_back();

	server.setRoot(str);
}
