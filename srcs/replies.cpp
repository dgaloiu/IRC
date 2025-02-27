#include <cstdarg>
#include <sstream>
#include <iomanip>
#include "replies.hpp"
#include "server.hpp"

void IRCServer::init_replies()
{
	this->replies[RPL_WELCOME] = ":Welcome to the Internet Relay Network <nick>!<user>@<host>";
	this->replies[RPL_NOTOPIC] = "<channel> :No topic is set";
	this->replies[RPL_TOPIC] = "<channel> :<topic>";
	this->replies[RPL_INVITING] = "<channel> <nick>";
	this->replies[RPL_CHANNELMODEIS] = "<channel> <mode> <mode params>";

	/*
	RPL_NAMREPLY
	
	"( "=" / "*" / "@" ) <channel>
			:[ "@" / "+" ] <nick> *( " " [ "@" / "+" ] <nick> )
		- "@" is used for secret channels, "*" for private
		channels, and "=" for others (public channels).
	*/
	this->replies[RPL_NAMREPLY] = "<chanprefix> <channel> :<nickprefix><nick>";
	this->replies[RPL_ENDOFNAMES] = "<channel> :End of NAMES list";


	this->replies[ERR_NOSUCHNICK] = "<nickname> :No such nick/channel";
	this->replies[ERR_NOSUCHSERVER] = "<server name> :No such server";
	this->replies[ERR_NOSUCHCHANNEL] = "<channel name> :No such channel";
	this->replies[ERR_NONICKNAMEGIVEN] = ":No nickname given";
	this->replies[ERR_ERRONEUSNICKNAME] = "<nick> :Erroneous nickname";
	this->replies[ERR_NICKNAMEINUSE] = "<nick> :Nickname is already in use";
	this->replies[ERR_NICKCOLLISION] = "<nick> :Nickname collision KILL from <user>@<host>";
	this->replies[ERR_UNAVAILRESOURCE] = "<nick/channel> :Nick/channel is temporarily unavailable";
	this->replies[ERR_NOTONCHANNEL] = "<channel> :You're not on that channel";
	this->replies[ERR_NEEDMOREPARAMS] = "<command> :Not enough parameters";
	this->replies[ERR_ALREADYREGISTRED] = ":Unauthorized command (already registered)";
	this->replies[ERR_RESTRICTED] = ":Your connection is restricted!";
	this->replies[ERR_UNKNOWNCOMMAND] = "<command> :Unknown command";
	this->replies[ERR_UNKNOWNMODE] = "<char> :is unknown mode char to me for <channel>";
	this->replies[ERR_INVITEONLYCHAN] = "<channel> :Cannot join channel (+i)";
	this->replies[ERR_CHANNELISFULL] = "<channel> :Cannot join channel (+l)";
	this->replies[ERR_BADCHANNELKEY] = "<channel> :Cannot join channel (+k)";
	this->replies[ERR_CHANOPRIVSNEEDED] = "<channel> :You're not channel operator";
	this->replies[ERR_NOCHANMODES] = "<channel> :Channel doesn't support modes";
	this->replies[ERR_USERONCHANNEL] = "<user> <channel> :is already on channel";
	this->replies[ERR_USERNOTINCHANNEL] = "<nick> <channel> :They aren't on that channel";
	this->replies[ERR_NOTREGISTERED] = ":You have not registered";
}

void IRCServer::send_cli_msg(Client &src, int socket, std::string &msg)
{
	//std::cout << "socket : " << socket << std::endl;
	std::string rpl = ":" + src.nick + "!" + src.user + "@" + src.hostname + " ";
	rpl += msg;
	rpl += "\r\n";
	send(socket, rpl.c_str(), rpl.size(), 0);
}

void IRCServer::send_serv_msg(int socket, std::string &msg)
{
	std::cout << "socket : " << socket << std::endl;
	std::string rpl = ":" + this->name + " ";
	rpl += msg;
	rpl += "\r\n";
	send(socket, rpl.c_str(), rpl.size(), 0);
}

void IRCServer::send_reply(Client &cli, int rpl_code...)
{
	std::stringstream rpl_s;
	rpl_s << ":" << this->name << " ";
	rpl_s << std::setfill('0') << std::setw(3) << rpl_code  << " ";
	if (!cli.nick.empty())
		rpl_s << cli.nick;
	else
		rpl_s << "*";
	rpl_s << " ";
	std::string param_start = "<";
	std::string param_end = ">";
	va_list args;
	va_start(args, rpl_code);

	size_t i = 0;
	std::string &fmt = this->replies[rpl_code];
	while(i < fmt.size())
	{
		size_t istart = fmt.find(param_start, i);
		size_t iend = fmt.find(param_end, i);

		if (i < istart)
		{
			rpl_s << fmt.substr(i, istart - i);
			i = istart;
		}
		else if (istart < iend)
		{
			rpl_s << va_arg(args, char*);
			i = iend + 1;
		}
		else
		{
			rpl_s << fmt.substr(i);
			i = fmt.size();
		}
	}
	rpl_s << "\r\n";
	std::string rpl = rpl_s.str();
	send(cli.socket, rpl.c_str(), rpl.size(), 0);
	//std::cout << "(REP)" << rpl << std::endl;
}
