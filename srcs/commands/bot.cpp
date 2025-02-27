#include "Irc.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Commands.hpp"

static void botHour(Server *server, int const client_fd, std::map<const int, Client>::iterator it_client, std::string bot)
{
	std::stringstream ss;
	std::time_t t = std::time(NULL);
	std::tm *tm_local = std::localtime(&t);

	ss << "Current local time: " << tm_local->tm_hour << ":"
	   << tm_local->tm_min << ":" << tm_local->tm_sec;

	std::string time = ss.str();
	appendClientBuffer(server, client_fd, RPL_PRIVMSG(bot, bot, it_client->second.getNickname(), time));
}

static void botQuote(Server *server, int const client_fd, std::map<const int, Client>::iterator it_client, std::string bot)
{
	srand(time(NULL));
	int index = rand() % 3 + 1;

	std::string str;
	switch (index)
	{
	case 1:
		str = "The only way to do great work is to love what you do. - Steve Jobs";
		break;
	case 2:
		str = "In three words I can sum up everything I've learned about life: it goes on. - Robert Frost";
		break;
	case 3:
		str = "The future belongs to those who believe in the beauty of their dreams. - Eleanor Roosevelt";
		break;
	}
	appendClientBuffer(server, client_fd, RPL_PRIVMSG(bot, bot, it_client->second.getNickname(), str));
}

void bot(Server *server, int const client_fd, std::map<const int, Client>::iterator it_client, std::string bot_cmd)
{
	std::string bot = "TinBox";
	std::string validCmds[4] = {
		":HELP",
		":HOUR",
		":QUOTE",
	};

	for (size_t i = 0; i < bot_cmd.size(); i++)
		bot_cmd[i] = std::toupper(bot_cmd[i]);

	int index = 0;
	while (index < 4)
	{
		if (bot_cmd == validCmds[index])
			break;
		index++;
	}
	switch (index + 1)
	{
	case 1:
		appendClientBuffer(server, client_fd, RPL_PRIVMSG(bot, bot, it_client->second.getNickname(), "You can ask me the hour with the command 'HOUR' or get a random quotes using 'QUOTE'"));
		break;
	case 2:
		botHour(server, client_fd, it_client, bot);
		break;
	case 3:
		botQuote(server, client_fd, it_client, bot);
		break;
	default:
		appendClientBuffer(server, client_fd, RPL_PRIVMSG(bot, bot, it_client->second.getNickname(), "Unknown command, use 'HELP' for more info"));
	}
}
