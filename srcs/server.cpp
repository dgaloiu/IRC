#include <utility>
#include <cstring>
#include "server.hpp"


IRCServer::IRCServer(int port, std::string pass)
{
	this->name = "ircserv.24.bruh";
	this->serv_pass = pass;
	std::pair<std::string, climsg_func> msg_tuple[] = {std::make_pair("PASS",  &IRCServer::pass_cmd),
	std::make_pair("NICK",  &IRCServer::nick_cmd), std::make_pair("USER",  &IRCServer::user_cmd),
	std::make_pair("QUIT",  &IRCServer::quit_cmd), std::make_pair("PRIVMSG",  &IRCServer::privmsg_cmd), std::make_pair("NOTICE",  &IRCServer::notice_cmd),
	std::make_pair("PING",  &IRCServer::ping_cmd) , std::make_pair("PONG",  &IRCServer::pong_cmd),
	std::make_pair("JOIN",  &IRCServer::join_cmd) , std::make_pair("PART",  &IRCServer::part_cmd),
	std::make_pair("TOPIC",  &IRCServer::topic_cmd), std::make_pair("MODE",  &IRCServer::mode_cmd),
	std::make_pair("INVITE",  &IRCServer::invite_cmd), std::make_pair("KICK",  &IRCServer::kick_cmd)
	};
	for (size_t j = 0; j < (sizeof(msg_tuple) / sizeof(msg_tuple[0])); ++j)
		this->commands[msg_tuple[j].first] = msg_tuple[j].second;

	std::pair<char, ChanMode*> chanmode_tuple[] = {
	std::make_pair('i', new InviteOnlyMode()), std::make_pair('t', new TopicRestrictMode()),
	std::make_pair('k', new KeyMode()), std::make_pair('o', new ChanOpMode()),
	std::make_pair('l', new UserLimitMode())
	};
	for (size_t j = 0; j < (sizeof(chanmode_tuple) / sizeof(chanmode_tuple[0])); ++j)
		this->chanmodes[chanmode_tuple[j].first] = chanmode_tuple[j].second;

	this->init_replies();
	this->servfd = socket(AF_INET, SOCK_STREAM , 0);
	int optval = true;
	setsockopt(this->servfd,SOL_SOCKET,SO_REUSEADDR, &optval, sizeof(int));
	if (this->servfd == -1)
	{
		//std::cerr << "Failed to create Server socket" << std::endl;
		//int err = errno;
		//perror(strerror(err));
		throw "Failed to create Server socket";
	}
	this->serv_addr.sin_addr.s_addr = INADDR_ANY;
	this->serv_addr.sin_family = AF_INET;
	this->serv_addr.sin_port = htons(port);
	int bind_error = bind(this->servfd , (struct sockaddr *) &this->serv_addr , sizeof(this->serv_addr));
	if (bind_error == -1)
	{
		//std::cerr << "Failed to bind Server socket" << std::endl;
		//int err = errno;
		//perror(strerror(err));
		throw "Failed to bind Server socket";
	}
	listen(this->servfd , 5);
	pollfd re = {this->servfd, POLLIN, 0};
	this->cli_fds.push_back(re); // push back Pauline
	this->init_handler();
	std::cout << "Server Launching..." << std::endl;
}

IRCServer::~IRCServer()
{
	std::cout << "Server Quitting..." << std::endl;
	for (size_t j = 0; j < this->cli_fds.size(); ++j)
	{
		close(this->cli_fds[j].fd);
	}
	for (std::map<char, ChanMode*>::iterator it = this->chanmodes.begin(); it != this->chanmodes.end(); ++it)
	{
		delete it->second;
	}
}

void IRCServer::accept_client()
{
	Client new_cli;
	new_cli.cliaddrlen = 0;
	std::memset(&new_cli.cli_addr, 0, sizeof(new_cli.cli_addr));
	new_cli.socket = accept(this->servfd , (struct sockaddr *) &(new_cli).cli_addr , &(new_cli).cliaddrlen);
	if (new_cli.socket == -1)
	{
		std::cerr << "New client connexion failed\n";
		return ;
	}

	struct sockaddr_in hname;
	socklen_t namelen = sizeof(name);
	if(getsockname(new_cli.socket, (struct sockaddr*)&hname, &namelen) == -1) {
		std::cerr << "Could not getsockname" << std::endl;
	}
	new_cli.hostname = inet_ntoa(hname.sin_addr);

	
	new_cli.status =  NOT_CONNECTED;
	pollfd re = {new_cli.socket, POLLIN, 0};
	this->cli_fds.push_back(re);
	this->fd_cli_map[new_cli.socket] = new_cli;
}

void IRCServer::receive_message(Client &client, std::vector<std::string> tokens)
{
	std::string cmd = tokens[0];
	//toupper cmd;
	tokens.erase(tokens.begin());
	try
	{
		if (client.status != AUTHENTIFICATED && cmd.compare("QUIT") &&
			cmd.compare("PASS") && cmd.compare("NICK") && cmd.compare("USER") && cmd.compare("CAP"))
		{
			//std::cout << cmd << "," << client.status << "\n";
			this->send_reply(client, ERR_NOTREGISTERED);
		}
		else
		{
			(this->*commands.at(cmd))(client, tokens);
		}
	}
	catch (const std::out_of_range &e)
	{
		this->send_reply(client, ERR_UNKNOWNCOMMAND, cmd.c_str());
	}
}

int IRCServer::manage_message(Client &client)
{
	char buf[512];

	memset(buf, 0, 512);
	int msg_len = recv(client.socket, buf, 512, 0);
	if (msg_len <= 0) //client disconnect
	{
		std::vector<std::string> args;
		args.push_back("EOF From client");
		do_quit_cmd(client, args, false);
		return -1;
	}
	std::cout << "cli buff : -" << client.buf << "-\n";
	client.buf += buf;
	if (client.buf.size() >= 510)
	{
		client.buf.resize(510);
		client.buf += "\r\n";
		// std::cout << "after resize client.buf -" << client.buf << "-\n";
	}
	size_t found = 0;
	std::string mes_end = "\n";
	while ((found = client.buf.find(mes_end, found))
				!= std::string::npos)
	{
			//std::cout << "found is : " << found << std::endl;
			//std::cout << "Buff avant tronquage : -" << client.buf << "-\n";
			std::string out = client.buf.substr(0 , found);
			if (found + 1 <= client.buf.size())
			{
				client.buf = client.buf.substr(found + 1);
				//std::cout << "Buff apres tronquage : -" << client.buf << "-\n";
			}
			out.erase(out.size() - (out[out.size() - 1] == '\r'));
			std::cout << "out : -" << out << "-\n";
			
			//print vector
			if (out.size() == 0)
				return 0;
			std::vector<std::string> tokens = tokenize(out);
			for (size_t i = 0; i < tokens.size(); i++)
				std::cout << "'" << tokens[i] << "'"<< std::endl;
			std::cout << std::endl;

			receive_message(client, tokens);
			found = 0;
	}
	return 0;
}

int IRCServer::setOpOnChan(std::string &nick, Channel& chan, bool status)
{
	if (nick_fd_map.count(nick) == 0)
	{
		return -1;
	}
	Client& nop = this->fd_cli_map[this->nick_fd_map[nick]];
	if ((status == true && chan.getUserPriv(&nop) == Operator) ||
	(status == false && chan.getUserPriv(&nop) == Normal))
		return 0;
	
	if (status == true)
		chan.setUserPriv(&nop, Operator);
	else
		chan.setUserPriv(&nop, Normal);
	return 1;
}

Client const *IRCServer::getClient(const std::string& nick) const
{
	std::map<std::string, int>::const_iterator nickfdi;
	std::map<int, Client>::const_iterator fdclii;
	nickfdi = this->nick_fd_map.find(nick);
	if (nickfdi == this->nick_fd_map.end())
		return NULL;
	fdclii = this->fd_cli_map.find(nickfdi->second);
	if (fdclii == this->fd_cli_map.end())
		return NULL;
	return &fdclii->second;
}

void IRCServer::_debug()
{
	std::cerr << "\n---STATE---\n";
	std::cerr << "Nb Clients(W/ server): " << this-> cli_fds.size() << "\n";
	std::cerr << "socket serv: " << this->cli_fds[0].fd << "\n";
	std::map<int, Client>::iterator fdit;
	std::map<std::string, Channel>::iterator chanit;
	std::set<std::string>::iterator channameit;
	for (fdit = this->fd_cli_map.begin(); fdit != this->fd_cli_map.end(); ++fdit)
	{
		std::cout << "Nick: <" << fdit->second.nick << "> (socket: " << fdit->first << ")\n";
		std::cout << "\t in chan:";
		channameit = fdit->second.chan_list.begin();
		for (; channameit != fdit->second.chan_list.end(); ++channameit)
		{
			std::cout << " " << *channameit;
		}
		std::cout << "\n";
	}
	std::cout << "Channels: \n";
	for (chanit = this->channels.begin(); chanit != this->channels.end(); ++chanit)
	{
		std::cout << "\t" << chanit->second.getName() << \
		"(" << chanit->second.getUserNumber() << " users)"<< std::endl;
	}
	std::cerr << "-----------\n";
}


void IRCServer::run()
{

	int nb_events;
	extern bool g_sigint;

	while(g_sigint != 1)
	{
		nb_events = poll(this->cli_fds.data(), this->cli_fds.size(), 0);
		if (nb_events < 0)
		{
			std::cerr << "poll error\n";
			return ;
		}
		// New Connection
		if (this->cli_fds[0].revents & POLLIN)
		{
			
			this->accept_client();
		}
	
		for (size_t i = 1; i < this->cli_fds.size(); i++)
		{
			if (this->cli_fds[i].revents & POLLIN)
			{
				manage_message(this->fd_cli_map[this->cli_fds[i].fd]);
				this->_debug();
			}
		}
		std::map<std::string, Channel>::iterator chanit = this->channels.begin();
		for ( ; chanit != this->channels.end(); ++chanit)
		{
			if (chanit->second.getUserNumber() == 0)
			{
				this->channels.erase(chanit->first);
				break;
			}
		}
	}
}

std::vector<std::string> IRCServer::tokenize_targets(std::string str)
{
	//tokenizer for irc commands
	std::vector<std::string> tokens;
	size_t i = 0;
	size_t j = 0;

	while (i < str.size())
	{
		if (str[i] == ',')
		{
			tokens.push_back(str.substr(j, i - j));
			j = i + 1;
		}
		else if ((i == str.size() - 1 && str[i] == '\n') || str[i] == '\r')
			str.erase(i);
		++i;
	}
	tokens.push_back(str.substr(j, i - j));
	return tokens;
}

std::vector<std::string> IRCServer::tokenize(std::string str)
{
	std::vector<std::string> tokens;
	size_t i = 0;
	size_t j;

	//std::cerr << "COM: '" << str << "'\n";
	while (str[i] == ' ')
		++i;
	j = i;
	/*
	//IRC Prefix Part
	if (str[i] == ':')
	{
		++i;
		while(str[j] != ' ')
			++j;
		tokens.push_back(str.substr(i, j - i));
		i = j;
	}
	*/
	//main part
	while (i < str.size())
	{
		if (str[i] != ' ')
		{
			if (str[i] == ':') // We are at the end of msg
			{
				std::string end_part = str.substr(i + 1);
				if (!end_part.empty())
					tokens.push_back(end_part);
				return tokens;
			}
			j = i;
			while (j < str.size() && str[j] != ' ')
				++j;
			tokens.push_back(str.substr(i, j - i)); // Push everything before
			i = j;
		}
		++i;
	}
	return tokens;
}

void IRCServer::error_cmd(Client &client, std::string arg)
{
	std::string err = "ERROR ";
	err += arg;
	err += "\r\n";
	send(client.socket, err.c_str(), err.size(), 0);
}

bool IRCServer::is_special(char c)
{
	return(c == '[' || c == ']' || c == '\\' || c == '`' || c == '_' || c == '^' || c == '{' || c == '|' || c == '}');
}

bool IRCServer::is_nickname(std::string const &nickname)
{
	size_t i = 0;
	if (nickname.empty() || nickname.size() > 9)
		return (0);
	if (std::isalpha(nickname[0]) || is_special(nickname[0]))
	{
		while(++i < nickname.size())
		{
			if(!std::isalpha(nickname[i]) && !is_special(nickname[i]) && !std::isdigit(nickname[i]) && nickname[i] != '-')
				return (0);
		}
		return (1);
	}
	return (0);
}

bool IRCServer::is_channame(std::string const &channame)
{
	size_t i = 0;
	std::string chan_letter = "#+!&";

	if (channame.size() < 2 || channame.size() > 50)
		return (0);
	if (chan_letter.find(channame[0]) != std::string::npos)
	{
		while(++i < channame.size())
		{
			if (channame[i] == ' ' || channame[i] == ',' ||
				channame[i] == ':' || channame[i] == 9)
				return (0);
		}
		return (1);
	}
	return (0);
}
