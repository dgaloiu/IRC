#include "Server.hpp"

bool server_shutdown = false;

static void	signal_handler(int signal)
{
	(void)signal;
	server_shutdown = true;
}

int main (int argc, char **argv)
{
	if (argc == 3)
	{
		time_t rawtime;
		struct tm * timeinfo;

		time (&rawtime);
		timeinfo = localtime(&rawtime);

		signal(SIGINT, signal_handler);
		Server server(argv[1], argv[2], timeinfo);

		char filename[39] = "srcs/config/irc.config";
		server.readFromConfigFile(filename);
		
		server.setHints();
		if (server.fillServinfo(argv[1]) == FAILURE)
			return (FAILURE);
		server.launchServer();
		try
		{
			server.manageServerLoop();
		}
		catch(const std::exception& e) 
		{
			std::cerr << RED << "Caught exception : " << RESET << e.what() << std::endl;
		}

		return (SUCCESS);
	}
	else
	{
		std::cout << "Usage is ./ircserv [port] [password] :)" << std::endl;
		return (FAILURE);
	}
}