#include "server.hpp"

void IRCServer::part_cmd(Client &client, std::vector<std::string> args)
{
	/*
	RESPONSES:
		ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
        ERR_NOTONCHANNEL
	*/
	std::vector<std::string> chan_toks;
	std::map<std::string, Channel>::iterator chan_it;

	if (args.size() < 1)
	{
		send_reply(client, ERR_NEEDMOREPARAMS, "PART");
		return ;
	}
	chan_toks = tokenize_targets(args[0]);
	for (size_t i = 0; i < chan_toks.size(); ++i)
	{
		chan_it = this->channels.find(chan_toks[i]);
		if (chan_it == this->channels.end())
		{
			send_reply(client, ERR_NOSUCHCHANNEL, chan_toks[i].c_str());
		}
		else if (!chan_it->second.is_user_on(client))
		{
			send_reply(client, ERR_NOTONCHANNEL, chan_toks[i].c_str());
		}
		else
		{
			std::string part_msg = "PART " + chan_toks[i] + " :";
			if (args.size() > 1)
				part_msg += args[1];
			chan_it->second.send_msg(client, part_msg);
			chan_it->second.remove_user(client); // erase client from chan
		}
	}
}
