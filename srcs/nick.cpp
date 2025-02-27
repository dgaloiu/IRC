#include "server.hpp"

/*
RESPONSES:

		ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
		ERR_NICKNAMEINUSE               ERR_NICKCOLLISION
		ERR_UNAVAILRESOURCE (~)             ERR_RESTRICTED (not todo)
*/

void IRCServer::nick_cmd(Client &client, std::vector<std::string> args)
{
	// add nick bomb hehe ERR_NICKNAMEINUSE ERR_NICKCOLLISION
	
	if (args.empty())
	{
		this->send_reply(client, ERR_NONICKNAMEGIVEN);
		return;
	}
	if (!is_nickname(args[0])) // si >9 tronquer et prendre que le debut ?
	{
		this->send_reply(client, ERR_ERRONEUSNICKNAME, args[0].c_str());
		return;
	}
	if (this->nick_fd_map.count(args[0]))
	{
		this->send_reply(client, ERR_NICKNAMEINUSE, args[0].c_str());
		return;
	}
	if (client.status == PASS_OK)
	{
		client.status = NICK_OR_USER_AND_PASS_OK;
		client.nick = args[0];
	}
	else if (client.status == NICK_OR_USER_AND_PASS_OK)
	{
		client.nick = args[0];
		if (!client.user.empty())
		{
			client.status = AUTHENTIFICATED;
			this->nick_fd_map[client.nick] = client.socket;
			this->send_reply(client, RPL_WELCOME, client.nick.c_str(), client.user.c_str(), client.hostname.c_str());
		}
	}
	else if (client.status == AUTHENTIFICATED)
	{
		std::string msg = "NICK " + args[0];
		send_cli_msg(client, client.socket, msg);
		this->nick_fd_map[args[0]] = client.socket;
		this->nick_fd_map.erase(client.nick);
		client.nick = args[0];
	}
}
