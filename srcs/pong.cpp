#include "server.hpp"

void IRCServer::pong_cmd(Client &client, std::vector<std::string> args)
{
	(void) client;
	(void) args;
	/*Dunno : should be used in respond to ping to confim user is active*/
}
