#include "server.hpp"

void IRCServer::invite_cmd(Client &client, std::vector<std::string> args)
{
	/*
	RESPONSES:
		ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
		ERR_NOTONCHANNEL                ERR_USERONCHANNEL
		ERR_CHANOPRIVSNEEDED
		RPL_INVITING                    RPL_AWAY (not todo)
	*/

	std::map<std::string, Channel>::iterator chan_it;

	if (args.size() < 2)
	{
		send_reply(client, ERR_NEEDMOREPARAMS, "INVITE");
		return ;
	}

	if (nick_fd_map.count(args[0]) == 0)
	{
		this->send_reply(client, ERR_NOSUCHNICK, args[0].c_str());
		return ;
	}
	std::string message = "INVITE " + args[0] + " :" + args[1];
	Client& invited_cli = this->fd_cli_map[this->nick_fd_map[args[0]]];
	chan_it = this->channels.find(args[1]);
	if (chan_it == this->channels.end())
	{
		send_reply(client, RPL_INVITING, args[0].c_str(), args[1].c_str());
		send_cli_msg(client, this->nick_fd_map.at(args[0]), message);
		return ;
	}
	if (!chan_it->second.is_user_on(client))
		send_reply(client, ERR_NOTONCHANNEL, args[1].c_str());
	else if (chan_it->second.is_user_on(invited_cli))
		send_reply(client, ERR_USERONCHANNEL, args[0].c_str(), args[1].c_str());
	else if (chan_it->second.getModeFor(client, 'i').first != 0 && chan_it->second.getUserPriv(&client) != Operator)
		send_reply(client, ERR_CHANOPRIVSNEEDED, args[1].c_str());
	else
	{
		chan_it->second.add_invitation(invited_cli);
		send_reply(client, RPL_INVITING, args[0].c_str(), args[1].c_str());
		send_cli_msg(client, this->nick_fd_map.at(args[0]), message);
	}
}
