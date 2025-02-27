#ifndef CHANMODE_HPP
#define CHANMODE_HPP

#include <vector>
#include <string>

class IRCServer;
class Channel;

class ChanMode
{
	private:
		char name;
		int nb_param;
		ChanMode();
	public:
		ChanMode(char name, int nb_param);
		virtual ~ChanMode();
		char getName() const;
		virtual size_t getNbParam(char status) const;
		virtual int apply(IRCServer &serv, Channel &chan, Client& cli, bool status, std::vector<std::string> &params) = 0;
};

class InviteOnlyMode : public ChanMode
{
	public:
		InviteOnlyMode();
		int apply(IRCServer &serv, Channel &chan, Client& cli, bool status, std::vector<std::string> &params);
		//~InviteOnlyMode();
};

class TopicRestrictMode : public ChanMode
{
	public:
		TopicRestrictMode();
		int apply(IRCServer &serv, Channel &chan, Client& cli, bool status, std::vector<std::string> &params);
		//~TopicRestrictMode();
};

class KeyMode : public ChanMode
{
	public:
		KeyMode();
		int apply(IRCServer &serv, Channel &chan, Client& cli, bool status, std::vector<std::string> &params);
		//~KeyMode();
};

class ChanOpMode : public ChanMode
{
	public:
		ChanOpMode();
		int apply(IRCServer &serv, Channel &chan, Client& cli, bool status, std::vector<std::string> &params);
		//~ChanOpMode();
};

class UserLimitMode : public ChanMode
{
	public:
		UserLimitMode();
		size_t getNbParam(char status) const;
		int apply(IRCServer &serv, Channel &chan, Client& cli, bool status, std::vector<std::string> &params);
		//~UserLimitMode();
};


#endif
