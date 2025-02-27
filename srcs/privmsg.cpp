#include "server.hpp"

void IRCServer::do_msg_cmd(Client &client, std::vector<std::string> args, bool priv_or_notice)
{
	/*
	RESPONSES:
		ERR_NORECIPIENT (not todo)      ERR_NOTEXTTOSEND (not todo)
		ERR_CANNOTSENDTOCHAN (~)        ERR_NOTOPLEVEL (not todo)
		ERR_WILDTOPLEVEL (not todo)     ERR_TOOMANYTARGETS (not todo)
		ERR_NOSUCHNICK
		RPL_AWAY (not todo)
	*/
	std::string cmd_name = (priv_or_notice ? "NOTICE" : "PRIVMSG");

	if (args.size() < 2)
	{
		send_reply(client, ERR_NEEDMOREPARAMS, cmd_name.c_str());
		return;
	}
	std::string message;
	std::vector<std::string> targets = tokenize_targets(args[0]);

	for (size_t i = 0; i < targets.size(); ++i)
	{
		message = cmd_name + " " + targets[i] + " :" + args[1];

		if (this->nick_fd_map.count(targets[i]))
			this->send_cli_msg(client, this->nick_fd_map[targets[i]], message);
		else if (this->channels.count(targets[i]))
			this->channels[targets[i]].send_msg(client, message, false);
		else
			this->send_reply(client, ERR_NOSUCHNICK, targets[i].c_str());
	}
}

void IRCServer::privmsg_cmd(Client &client, std::vector<std::string> args)
{
	do_msg_cmd(client, args, 0);
}

void IRCServer::notice_cmd(Client &client, std::vector<std::string> args)
{
	do_msg_cmd(client, args, 1);
}
