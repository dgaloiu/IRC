#include <sstream>
#include "server.hpp"

void IRCServer::join_cmd(Client &client, std::vector<std::string> args)
{
	/*
	RESPONSES:
		ERR_NEEDMOREPARAMS              ERR_BANNEDFROMCHAN (~)
		ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
		ERR_CHANNELISFULL               ERR_BADCHANMASK (~)
		ERR_NOSUCHCHANNEL               ERR_TOOMANYCHANNELS
		ERR_TOOMANYTARGETS (not TODO)   ERR_UNAVAILRESOURCE (~)
		RPL_TOPIC
	*/
	std::vector<std::string> chan_toks;
	std::vector<std::string> key_toks;
	unsigned error;

	if (args.size() < 1)
	{
		send_reply(client, ERR_NEEDMOREPARAMS, "JOIN");
		return ;
	}
	if (!args[0].compare("0"))
	{
		std::set<std::string>::iterator channameit;
		channameit = client.chan_list.begin();
		for (; channameit != client.chan_list.end(); )
		{
			std::vector<std::string> param;
			param.push_back(*channameit);
			part_cmd(client, param);
			channameit = client.chan_list.begin();
		}
		return ;
	}

	if (args.size() > 1)
		key_toks = tokenize_targets(args[1]);
	chan_toks = tokenize_targets(args[0]);
	for (size_t i = 0; i < chan_toks.size(); ++i)
	{
		if (!is_channame(chan_toks[i]))
		{
			this->send_reply(client, ERR_NOSUCHCHANNEL, chan_toks[i].c_str());
			continue;
		}
		std::string join_msg = "JOIN :" + chan_toks[i];
		std::string key = (i < key_toks.size() ? key_toks[i] : "");
		if (this->channels.count(chan_toks[i]))
		{
			Channel &chan = this->channels[chan_toks[i]];

			//also must check mode if invite only, keypass,...
			error = chan.can_join_chan(client, key);
			if (error)
			{
				this->send_reply(client, error, chan_toks[i].c_str());
				continue;
			}
			chan.add_user(client, Normal);
			chan.remove_invitation(client);
		}

		else
		{
			//should create channel, add it to server channel list, user channels list and make him op
			this->channels[chan_toks[i]] = Channel(chan_toks[i], key);
			this->channels[chan_toks[i]].add_user(client, Operator);
		}
		this->channels[chan_toks[i]].send_msg(client, join_msg);
		std::string const &topic = this->channels[chan_toks[i]].getTopic();
		std::map<Client*, user_privileges> const &users = this->channels[chan_toks[i]].getUsers();
		std::map<Client*, user_privileges>::const_iterator uit;
		if (!topic.empty())
			this->send_reply(client, RPL_TOPIC, chan_toks[i].c_str(), topic.c_str());
		for (uit = users.begin(); uit != users.end(); ++uit)
		{
			this->send_reply(client, RPL_NAMREPLY, "=", chan_toks[i].c_str(),
			uit->second == Normal ? "": "@", uit->first->nick.c_str());
		}
		this->send_reply(client, RPL_ENDOFNAMES, chan_toks[i].c_str());
	}

}
