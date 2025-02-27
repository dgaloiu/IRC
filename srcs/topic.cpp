#include "server.hpp"

void IRCServer::topic_cmd(Client &client, std::vector<std::string> args)
{
	/*
	RESPONSES:
		ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
		RPL_NOTOPIC                     RPL_TOPIC
		ERR_CHANOPRIVSNEEDED            ERR_NOCHANMODES (~)
	*/

	if (args.size() < 1)
	{
		send_reply(client, ERR_NEEDMOREPARAMS, "TOPIC");
		return ;
	}
	std::map<std::string, Channel>::iterator chan_it;
	std::map<std::string, u_int8_t>::iterator cli_it;
	chan_it = this->channels.find(args[0]);
	if (chan_it == this->channels.end())
	{
		this->send_reply(client, ERR_NOSUCHCHANNEL, args[0].c_str());
		return ;
	}

	if (args.size() == 1)
	{
		std::string const &chantopic = chan_it->second.getTopic();
		this->send_reply(client, chantopic.empty() ? RPL_NOTOPIC : RPL_TOPIC, chan_it->first.c_str(), chantopic.c_str());
	}
	else if (args.size() > 1)
	{
		if (!chan_it->second.is_user_on(client))
			this->send_reply(client, ERR_NOTONCHANNEL, chan_it->first.c_str());
		else if (chan_it->second.getModeFor(client, 't').first != 0 &&
				(chan_it->second.getUserPriv(&client) != Operator))
			this->send_reply(client, ERR_CHANOPRIVSNEEDED, chan_it->first.c_str());
		else
		{
			std::string topic_msg = "TOPIC " + args[0] + " :" + args[1];
			chan_it->second.setTopic(args[1]);
			chan_it->second.send_msg(client, topic_msg);
		}

	}
	

}
