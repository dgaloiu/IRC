#include <sstream>
#include <stdint.h>
#include "server.hpp"



ChanMode::ChanMode(char name, int nb_param) : name(name), nb_param(nb_param)
{
}

ChanMode::~ChanMode()
{
}


char ChanMode::getName() const { return this->name; }

size_t ChanMode::getNbParam(char status) const
{
	(void) status;
	return this->nb_param;
}

size_t UserLimitMode::getNbParam(char status) const
{
	if (status == '+')
		return ChanMode::getNbParam(status);
	return ChanMode::getNbParam(status) - 1;
}

InviteOnlyMode::InviteOnlyMode() : ChanMode('i', 0)
{
}

TopicRestrictMode::TopicRestrictMode() : ChanMode('t', 0)
{
}

KeyMode::KeyMode() : ChanMode('k', 1)
{
}

UserLimitMode::UserLimitMode() : ChanMode('l', 1)
{
}


ChanOpMode::ChanOpMode() : ChanMode('o', 1)
{
}

int InviteOnlyMode::apply(IRCServer& serv, Channel &chan, Client& cli, bool status, std::vector<std::string>& args)
{
	(void)serv;
	(void)status;
	(void)chan;
	(void)args;
	(void)cli;
	if (chan.is_mode_same_on_chan(*this, status, args))
		return 0;
	chan.setMode(this->getName(), status, args);
	return 1;
}

int TopicRestrictMode::apply(IRCServer& serv, Channel &chan, Client& cli, bool status, std::vector<std::string>& args)
{
	(void)serv;
	(void)status;
	(void)chan;
	(void)args;
	(void)cli;
	if (chan.is_mode_same_on_chan(*this, status, args))
		return 0;
	chan.setMode(this->getName(), status, args);
	return 1;
}

int KeyMode::apply(IRCServer& serv, Channel &chan, Client& cli, bool status, std::vector<std::string>& args)
{
	(void)serv;
	(void)status;
	(void)chan;
	(void)args;
	(void)cli;
	if (chan.is_mode_same_on_chan(*this, status, args))
		return 0;
	std::pair<char, std::vector<std::string> > key_status = chan.getModeFor(cli, 'k');
	if (key_status.first != 'k') // pass not set
	{
		if (status == true)
			chan.setPass(args[0]);
		else
			return -1;
	}
	else //pass already set
	{
		if (status == true)
		{
			return -1;
		}
		else
		{
			if (key_status.second[0].compare(args[0]) != 0) // if pass ok
				return -1;
			std::string nstr;
			chan.setPass(nstr);
		}
	}
	chan.setMode(this->getName(), status, args);
	return 1;
}

int ChanOpMode::apply(IRCServer& serv, Channel &chan, Client& cli, bool status, std::vector<std::string>& args)
{
	Client const *cli_ptr = serv.getClient(args[0]);
	if (!cli_ptr)
	{
		serv.send_reply(cli, ERR_NOSUCHNICK, args[0].c_str());
		return -1;
	}
	return serv.setOpOnChan(args[0], chan, status);
}

int UserLimitMode::apply(IRCServer& serv, Channel &chan, Client& cli, bool status, std::vector<std::string>& args)
{
	(void)serv;
	(void)cli;

	if (status == false)
	{
		chan.setUserLimit(SIZE_MAX);
	}
	else
	{
		size_t n;
		std::stringstream ss;
		ss << args[0];
		ss >> n;
		if (n <= 0 || n < chan.getUserNumber())
			return -1;
		if (chan.getUserLimit() == n)
			return 0;
		chan.setUserLimit(n);
	}
	chan.setMode(this->getName(), status, args);
	return 1;
}
