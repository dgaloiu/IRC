#include "Server.hpp"
#include "Colors.hpp"
#include "Commands.hpp"

int Server::manageServerLoop()
{
	std::vector<pollfd> poll_fds;
	pollfd server_poll_fd;

	server_poll_fd.fd = _server_socket_fd;
	server_poll_fd.events = POLLIN;

	poll_fds.push_back(server_poll_fd);

	while (server_shutdown == false)
	{
		std::vector<pollfd> new_pollfds; // tmp struct hosting potential newly-created fds

		if (poll((pollfd *)&poll_fds[0], (unsigned int)poll_fds.size(), -1) <= SUCCESS) // -1 == no timeout
		{
			if (errno == EINTR)
				break;
			std::cerr << RED << "[Server] Poll error" << RESET << std::endl;
			throw;
		}

		std::vector<pollfd>::iterator it = poll_fds.begin();
		while (it != poll_fds.end())
		{
			if (it->revents & POLLIN) // => If the event that occured is a POLLIN (aka "data is ready to recv() on this socket")
			{
				if (it->fd == _server_socket_fd)
				{
					if (this->createClientConnexion(poll_fds, new_pollfds) == CONTINUE)
						continue;
				}
				else // => If the dedicated fd for the Client/Server connection already exists
				{
					if (this->handleExistingConnexion(poll_fds, it) == BREAK)
						break;
				}
			}
			else if (it->revents & POLLOUT) // = "Alert me when I can send() data to this socket without blocking."
			{
				if (handlePolloutEvent(poll_fds, it, it->fd) == BREAK)
					break;
			}
			else if (it->revents & POLLERR)
			{
				if (handlePollerEvent(poll_fds, it) == BREAK)
					break;
				else
					return (FAILURE);
			}
			it++;
		}
		poll_fds.insert(poll_fds.end(), new_pollfds.begin(), new_pollfds.end()); // Add the range of NEW_pollfds in poll_fds (helps recalculating poll_fds.end() in the for loop)
	}

	return (SUCCESS);
}

static int acceptSocket(int listenSocket)
{
	sockaddr_in client;
	socklen_t addr_size = sizeof(sockaddr_in);
	return (accept(listenSocket, (sockaddr *)&client, &addr_size));
}

static void tooManyClients(int client_socket)
{
	std::cout << RED << ERR_FULL_SERV << RESET << std::endl;
	send(client_socket, ERR_FULL_SERV, strlen(ERR_FULL_SERV) + 1, 0);
	close(client_socket);
}

int Server::createClientConnexion(std::vector<pollfd> &poll_fds, std::vector<pollfd> &new_pollfds)
{
	int client_sock = acceptSocket(_server_socket_fd); // Accepts the socket and returns a dedicated fd for this new Client-Server connexion
	if (client_sock == -1)
	{
		std::cerr << RED << "[Server] Accept() failed" << RESET << std::endl;
		return (CONTINUE);
	}
	if (poll_fds.size() - 1 < MAX_CLIENT_NB)
		addClient(client_sock, new_pollfds); // Beware, here we push the new client_socket in NEW_pollfds
	else
		tooManyClients(client_sock);
	return (SUCCESS);
}

static void print(std::string type, int client_socket, char *message)
{
	if (message)
		std::cout << std::endl
				  << type << client_socket << " << "
				  << BLUE << message << RESET;
}

int Server::handleExistingConnexion(std::vector<pollfd> &poll_fds, std::vector<pollfd>::iterator &it)
{
	Client *client;
	client = getClient(this, it->fd);
	char message[BUF_SIZE_MSG];
	int read_count;

	memset(message, 0, sizeof(message));
	read_count = recv(it->fd, message, BUF_SIZE_MSG, 0); // Retrieves the Client's message

	if (read_count <= FAILURE) // when recv returns an error
	{
		std::cerr << RED << "[Server] Recv() failed [456]" << RESET << std::endl;
		delClient(poll_fds, it, it->fd);
		return (BREAK);
	}
	else if (read_count == 0) // when a client disconnects
	{
		std::cout << "[Server] A client just disconnected\n";
		delClient(poll_fds, it, it->fd);
		return (BREAK);
	}
	else
	{
		print("[Client] Message received from client ", it->fd, message); // si affichage incoherent regarder ici
		client->setReadBuffer(message);

		if (client->getReadBuffer().find("\r\n") != std::string::npos)
		{
			try
			{
				parseMessage(it->fd, client->getReadBuffer());
				if (client->getReadBuffer().find("\r\n"))
					client->getReadBuffer().clear();
			}
			catch (const std::exception &e)
			{
				std::cout << "[SERVER] Caught exception : ";
				std::cerr << e.what() << std::endl;
				if (client->isRegistrationDone() == true)
					client->setDeconnexionStatus(true);
				return (BREAK);
			}
		}
	}
	return (SUCCESS);
}

int Server::handlePolloutEvent(std::vector<pollfd> &poll_fds, std::vector<pollfd>::iterator &it, const int current_fd)
{
	Client *client = getClient(this, current_fd);
	if (!client)
		std::cout << "[Server] Did not found connexion to client sorry" << std::endl;
	else
	{
		sendServerRpl(current_fd, client->getSendBuffer());
		client->getSendBuffer().clear();
		if (client->getDeconnexionStatus() == true)
		{
			delClient(poll_fds, it, current_fd);
			return (BREAK);
		}
	}
	return (SUCCESS);
}

int Server::handlePollerEvent(std::vector<pollfd> &poll_fds, std::vector<pollfd>::iterator &it)
{
	if (it->fd == _server_socket_fd)
	{
		std::cerr << RED << "[Server] Listen socket error" << RESET << std::endl;
		return (FAILURE);
	}
	else
	{
		delClient(poll_fds, it, it->fd);
		return (BREAK);
	}
}
