#include <signal.h>
#include "server.hpp"

bool g_sigint = 0;

void int_handler(int sig)
{
	(void)sig;
	g_sigint = 1;
	return ;
}

void IRCServer::init_handler()
{
	signal(SIGINT, int_handler);
}
