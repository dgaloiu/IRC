#include "server.hpp"

int main(int ac , char *av[])
{
	(void) ac;
	(void) av;

	if (ac != 3 || (av[1][0] == 0 || av[2][0] == 0))
	{
		std::cout << "USAGE: ./ircserv [port] [pass]" << std::endl;
		return -1;
	}

	// check parsing not just by atoi ?
	try
	{
		IRCServer serv(atoi(av[1]), av[2]);
		serv.run();
	}
	catch(char const* e)
	{
		std::cerr << e << '\n';
	}
	
	return 0;
}
