// ************************************************************************** //
//                                                                            //
//                Supervisor.cpp                                              //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     : Jules Longin                                  //
//                                                                            //
// ************************************************************************** //

# include "Supervisor.hpp"
# include "Exceptions.hpp"

Supervisor::Supervisor() : _size(0) { std::memset(_fds, 0, sizeof(_fds)); }
Supervisor::~Supervisor() {}

Supervisor::Supervisor( const Supervisor & s ) { *this = s; }

Supervisor::Supervisor( const std::vector<Server> & servers ) : _size(servers.size())
{
	std::memset(_fds, 0, sizeof(_fds));

	for (size_t i = 0; i < _size; ++i)
	{
		_fds[i].fd = servers[i].getSocket();
		_fds[i].events = POLLIN;
	}
}

Supervisor	&Supervisor::operator = ( const Supervisor & s )
{
	if (this != &s)
	{
		for (int i = 0; i < FDS_SIZE; ++i)
			_fds[i] = s.getFdsX(i);
		_size = s.getSize();
	}
	return *this;
}

// Method

void	Supervisor::execution( void )
{
	while (true)
	{
		if (poll(_fds, _size, 0) == -1)
			throw FailedPoll();
		for (size_t i = 0; i < _size; ++i)
		{
			;
		}
	}
}

// Getter

size_t	Supervisor::getSize() const { return _size; }
struct pollfd	Supervisor::getFdsX( int idx ) const { return _fds[idx]; }

/*
		// 3 minutes waiting for something
		int nfds = 1;
		while (1)
		{
			printf("Waiting the poll()...\n");
			if (poll(fds, nfds, 0) == -1)
				throw FailedPoll();
			for (int i = 0; i < nfds; ++i)
			{
				if (fds[i].revents == 0)
					continue ;
				if (fds[i].revents != POLLIN)
				{
					printf("Error 1");
					return 1;
				}
				if (fds[i].fd == servers[0].getSocket())
				{
					printf("Listening socket");
					int client_fd = accept(servers[0].getSocket(), NULL, NULL);
					if (client_fd == -1)
						throw FailedAccept();
					printf("New incoming connexion");
					fds[nfds].fd = client_fd;
					fds[nfds].events = POLLIN;
					nfds++;
				}
				else
				{
					char		buffer[80];
					int rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc == -1)
						throw FailedRecv();
					if (rc == 0)
					{
						printf("Connexion closed.");
						break ;
					}
					rc = send(fds[i].fd, buffer, rc, 0);
					if (rc == -1)
						throw FailedSend();
				}
			}
		}
		for (int i = 0; i < nfds; ++i)
			if (fds[i].fd >= 0)
				close(fds[i].fd);
*/
