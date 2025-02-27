#include "server.hpp"

void IRCServer::ping_cmd(Client &client, std::vector<std::string> args)
{
	if (args.size() < 1)
	{
		send_reply(client, ERR_NEEDMOREPARAMS, "PING");
	}
	else
	{
		if (args.size() == 1 && (!args[0].compare(this->name) /*|| !args[1].compare("127.0.0.1")*/)) // Can be done because sing server network !
		{
			std::string rpl;
			rpl += "PONG " + this->name + " :" + args[0];
			send_serv_msg(client.socket, rpl);
		}
		else if (args.size() >= 2)
			send_reply(client, ERR_NOSUCHSERVER, args[1].c_str());
	}
}
