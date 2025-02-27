#include "server.hpp"

void IRCServer::user_cmd(Client &client, std::vector<std::string> args)
{
	if (client.status == AUTHENTIFICATED)
	{
		this->send_reply(client, ERR_ALREADYREGISTRED);
		return;
	}
	if (args.size() < 4)
	{
		this->send_reply(client, ERR_NEEDMOREPARAMS, "USER");
		return;
	}

	if (client.status == PASS_OK)
	{
		client.user = args[0];
		client.realname = args[3];
		client.status = NICK_OR_USER_AND_PASS_OK;
	}
	else if (client.status == NICK_OR_USER_AND_PASS_OK)
	{
		client.user = args[0];
		client.realname = args[3];
		if (!client.nick.empty())
		{
			client.status = AUTHENTIFICATED;
			this->nick_fd_map[client.nick] = client.socket;
			this->send_reply(client, RPL_WELCOME, client.nick.c_str(), client.user.c_str(), client.hostname.c_str());
		}
	}
}
