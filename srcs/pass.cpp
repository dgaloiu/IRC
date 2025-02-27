#include "server.hpp"

void IRCServer::pass_cmd(Client &client, std::vector<std::string> args)
{
	if (client.status != AUTHENTIFICATED)
	{
		if (args.size() < 1)
		{
			this->send_reply(client, ERR_NEEDMOREPARAMS, "PASS");
			return;
		}
		if (args[0].compare(this->serv_pass) == 0) // good pass
		{
			if (client.status == NOT_CONNECTED)
				client.status = PASS_OK;
		}
		else // bad pass
		{
			client.status = NOT_CONNECTED;
		}
	}
	else
		this->send_reply(client, ERR_ALREADYREGISTRED);
}
