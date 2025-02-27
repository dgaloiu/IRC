#include <unistd.h>
#include "channel.hpp"
#include "replies.hpp"

Channel::Channel() {}

Channel::Channel(std::string &name, std::string pass)
: name(name), pass(pass), topic("")
{
	this->user_limit = modes.max_size();
	if (!pass.empty())
	{
		std::vector<std::string> keypars(1, pass);
		setMode('k', true, keypars);
	}
}

/*
Channel::Channel(Channel const &chan)
{
	*this = chan;
}

Channel& Channel::operator=(Channel const &chan)
{
	if (this != &chan)
	{
		this->name = chan.name;
		this->topic = chan.topic;
		this->pass = chan.pass;
		this->user_limit= chan.user_limit;
		this->users= chan.users;
		this->invited= chan.invited;
		this->i_flag = chan.i_flag;
		this->t_flag = chan.t_flag;
	}
	return *this;
}
*/

Channel::~Channel()
{
}

std::string const &Channel::getName() const
{
	return this->name;
}

int Channel::checkPass(std::string const &key) const
{
	return this->pass.compare(key);
}

std::string const &Channel::getTopic() const
{
	return this->topic;
}

std::map<Client*, user_privileges> const &Channel::getUsers() const
{
	return this->users;
}


size_t Channel::getUserLimit()
{
	return this->user_limit;
}

size_t Channel::getUserNumber()
{
	return this->users.size();
}

std::pair<char, std::vector<std::string> > Channel::getModeFor(Client &cli, char m)
{
	const std::string secret_modes = "kl";

	std::pair<char, std::vector<std::string> > mode;
	if (modes.count(m) == 0)
		return mode;
	mode.first = m;
	mode.second = this->modes[m];
	if (!this->modes[m].empty() && secret_modes.find(m) != std::string::npos
		&& !is_user_on(cli))
	{
		for (size_t i = 0; i < mode.second.size(); ++i)
			mode.second[i] = '*';
	}
	return mode;
}

std::map<char, std::vector<std::string> > Channel::getModeFor(Client &cli, std::string mstr)
{
	std::map<char, std::vector<std::string> > chmodes;
	for (size_t j = 0; mstr[j] != '\0'; ++j)
		chmodes.insert(this->getModeFor(cli, mstr[j]));
	return chmodes;
}

std::map<char, std::vector<std::string> > Channel::getModeFor(Client &cli)
{
	std::map<char, std::vector<std::string> > chmodes;
	std::map<char, std::vector<std::string> >::iterator chit;

	for (chit = this->modes.begin(); chit != this->modes.end(); ++chit)
		chmodes.insert(this->getModeFor(cli, chit->first));
	return chmodes;
}

bool Channel::is_mode_same_on_chan(ChanMode &chm, bool status, std::vector<std::string>& args)
{
	std::map<char, std::vector<std::string> >::iterator it;
	it = this->modes.find(chm.getName());
	if ((it == this->modes.end() && status == false))
		return true;
	if (it != this->modes.end() && status == true)
	{
		if (args.size() != it->second.size()) //Parse Error ?
			return false;
		for (size_t j = 0; j < it->second.size(); ++j)
		{
			if (args[j].compare(it->second[j]) != 0)
				return false;
		}
		return true;
	}
	return false;
}


void Channel::setMode(char m, bool status, std::vector<std::string> params)
{
	if (status == true)
		this->modes[m] = params;
	else if (status == false && this->modes.count(m))
		this->modes.erase(m);
}

unsigned Channel::can_join_chan(Client &cli, std::string &key)
{
	if (this->modes.count('l'))
	{
		if (this->user_limit > 0 && this->user_limit >= this->users.size())
		//this->send_reply(cli, ERR_CHANNELISFULL, this->name.c_str());
			return ERR_CHANNELISFULL;
	}
	if (this->modes.count('i'))
	{
		if (this->invited.count(&cli) == 0)
		{
			//this->send_reply(cli, ERR_INVITEONLYCHAN, this->name.c_str());
			return ERR_INVITEONLYCHAN;
		}
	}
	if (this->modes.count('k') &&
		!this->modes['k'][0].empty() && this->modes['k'][0].compare(key))
	{
		//this->send_reply(cli, ERR_CHANNELISFULL, this->name.c_str());
		return ERR_BADCHANNELKEY;
	}
	return 0;
}

void Channel::add_user(Client &cli, user_privileges up)
{
	this->users[&cli] = up;
	cli.chan_list.insert(this->name);
}

void Channel::remove_user(Client &cli)
{
	if (cli.chan_list.count(this->name))
	{
		cli.chan_list.erase(this->name);
		this->users.erase(&cli);
	}
}

void Channel::add_invitation(Client &cli)
{
	this->invited.insert(&cli);
}

void Channel::remove_invitation(Client &cli)
{
	this->invited.erase(&cli);
}

void Channel::send_msg(Client &src, std::string &msg, bool send_to_client)
{
	bool user_eq_cli;
	for (std::map<Client*, user_privileges>::iterator it = this->users.begin(); it!=this->users.end(); ++it)
	{
		user_eq_cli = src.nick.compare(it->first->nick) == 0;
		if (user_eq_cli == false || (user_eq_cli == true && send_to_client == true))
		{
			std::string rpl = ":" + src.nick + "!" + src.user + "@" + src.hostname + " ";
			rpl += msg;
			rpl += "\r\n";
			send(it->first->socket, rpl.c_str(), rpl.size(), 0);
		}
	}
}

bool Channel::is_user_on(Client &cli)
{
	return (this->users.find(&cli) != this->users.end());
}

bool Channel::is_user_invited(Client &cli)
{
	return (this->invited.find(&cli) != this->invited.end());
}

user_privileges Channel::getUserPriv(Client *cli_ptr)
{
	return this->users[cli_ptr];
}

void Channel::setUserPriv(Client *cli_ptr, user_privileges up)
{
	this->users[cli_ptr] = up;
}

void Channel::setPass(std::string const &pass)
{
	this->pass = pass;
}

void Channel::setTopic(std::string const &topic)
{
	this->topic = topic;
}

void Channel::setUserLimit(size_t n)
{
	this->user_limit = n;
}
