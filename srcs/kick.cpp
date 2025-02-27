#include "server.hpp"

void IRCServer::kick_cmd(Client &client, std::vector<std::string> args)
{
	/*
	RESPONSES:
		ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
		ERR_BADCHANMASK                 ERR_CHANOPRIVSNEEDED
		ERR_USERNOTINCHANNEL            ERR_NOTONCHANNEL
	*/

	if (args.size() < 2)
	{
		send_reply(client, ERR_NEEDMOREPARAMS, "KICK");
		return ;
	}
	std::vector<std::string> chan_toks = tokenize_targets(args[0]);
	std::vector<std::string> nick_toks = tokenize_targets(args[1]);
	if (chan_toks.size() == 1)
	{
		chan_toks.insert(chan_toks.begin(), nick_toks.size() - 1, chan_toks[0]);
	}

	for (size_t j=0; j < nick_toks.size(); ++j)
	{
		if (this->channels.count(chan_toks[j]))
		{
			Channel &chan = this->channels[chan_toks[j]];
			if (!chan.is_user_on(client))
				send_reply(client, ERR_NOTONCHANNEL, chan_toks[0].c_str());
			else if (chan.getUserPriv(&client) != Operator)
				send_reply(client, ERR_CHANOPRIVSNEEDED, chan_toks[0].c_str());
			else if (nick_fd_map.count(nick_toks[j]) == 0)
				send_reply(client, ERR_NOSUCHNICK, nick_toks[j].c_str());
			else
			{
				Client& kicked_cli = this->fd_cli_map[this->nick_fd_map[nick_toks[j]]];
				if (!chan.is_user_on(kicked_cli))
					send_reply(client, ERR_USERNOTINCHANNEL, args[0].c_str(), args[1].c_str());
				else
				{
					std::string msg = "KICK " + chan.getName() + " " + nick_toks[j];
					if (args.size() >= 3)
						msg += " :" + args[2];
					chan.send_msg(kicked_cli, msg);
					chan.remove_user(kicked_cli);
				}
			}
		}
		else
			send_reply(client, ERR_NOSUCHCHANNEL, chan_toks[0].c_str());
	}
}
