#include <algorithm>
#include "server.hpp"

void IRCServer::do_quit_cmd(Client &client, std::vector<std::string> args, bool send_closing)
{
	std::vector<std::string> part_pars;
	std::set<std::string>::iterator channameit;
	channameit = client.chan_list.begin();
	for (; channameit != client.chan_list.end(); )
	{
		
		part_pars.push_back(*channameit);
		if (!args.empty())
			part_pars.push_back(args[0]);
		part_cmd(client, part_pars);
		channameit = client.chan_list.begin();
	}

	if (send_closing)
	{
		std::string end_co_error = ":Closing Link: " +
		client.nick + "[" + (!client.nick.empty()? client.nick : "unknown") + "@" + client.hostname + "]" + // nick + user? 
		"(" + (!args.empty() ? args[0] : "") + ")";
		this->error_cmd(client, end_co_error);
	}

	for (size_t i  = 0; i < this->cli_fds.size(); ++i) // search and delete fd and client
	{
		if (this->cli_fds[i].fd == client.socket)
		{
			close(this->cli_fds[i].fd);
			this->cli_fds.erase(this->cli_fds.begin() + i);
			break;
		}
	}
	this->nick_fd_map.erase(client.nick);
	this->fd_cli_map.erase(client.socket);
}

void IRCServer::quit_cmd(Client &client, std::vector<std::string> args)
{
	do_quit_cmd(client, args, true);
}
