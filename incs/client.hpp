#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <arpa/inet.h>
#include <set>
#include <string>

enum cli_connection_status
{
	NOT_CONNECTED,
	PASS_OK,
	NICK_OR_USER_AND_PASS_OK,
	AUTHENTIFICATED
};

struct Client
{
	int	socket;
	std::string buf;
	struct sockaddr_in cli_addr;
	socklen_t cliaddrlen;
	cli_connection_status status;
	std::string nick;
	std::string user;
	std::string realname;
	std::string hostname;
	std::set<std::string> chan_list;

};
#endif
