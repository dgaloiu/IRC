#ifndef SERVER_HPP
#define SERVER_HPP
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <vector>
#include <map>
#include "replies.hpp"
#include "channel.hpp"
#include "chanmode.hpp"
#include "client.hpp"

class IRCServer
{

	private:
		typedef void (IRCServer::*climsg_func)(Client&, std::vector<std::string>);

		std::string name;
		int servfd;
		std::string serv_pass; //to crypt ? x)
		struct sockaddr_in serv_addr;
		std::vector<pollfd> cli_fds;
		std::map<int, Client> fd_cli_map;
		std::map<std::string, int> nick_fd_map;
		std::map<int, std::string> replies;
		std::map<std::string, Channel> channels;
		std::map<std::string, climsg_func> commands;
		std::map<char, ChanMode*> chanmodes;
		std::vector<std::string> tokenize(std::string str);
		std::vector<std::string> tokenize_targets(std::string str);
		void do_msg_cmd(Client &client, std::vector<std::string> args, bool priv_or_notice);
		void do_quit_cmd(Client &client, std::vector<std::string> args, bool send_closing);
		void error_cmd(Client &client, std::string arg);
		void _debug();
		void init_handler();
		void init_replies();
	
	public:
		void send_reply(Client &cli, int rpl_code...);
		void send_cli_msg(Client &src, int socket, std::string &msg);
		void send_serv_msg(int socket, std::string &msg);
		IRCServer(int port, std::string pass);
		~IRCServer();
		void run();
		void accept_client();
		int manage_message(Client &client);
		void receive_message(Client &client, std::vector<std::string> tokens);
		int setOpOnChan(std::string &nick, Channel& chan, bool status);
		Client const *getClient(const std::string& nick) const;
		bool is_special(char c);
		bool is_nickname(std::string const &nickname);
		bool is_channame(std::string const &channame);
		//
		//void send_ping();
		//void remove_fd_client(int fd);
		//clients commands
		void pass_cmd(Client &client, std::vector<std::string> args);
		void nick_cmd(Client &client, std::vector<std::string> args);
		void user_cmd(Client &client, std::vector<std::string> args);
		void quit_cmd(Client &client, std::vector<std::string> args);
		void privmsg_cmd(Client &client, std::vector<std::string> args);
		void notice_cmd(Client &client, std::vector<std::string> args);
		void join_cmd(Client &client, std::vector<std::string> args);
		void part_cmd(Client &client, std::vector<std::string> args);
		void ping_cmd(Client &client, std::vector<std::string> args);
		void pong_cmd(Client &client, std::vector<std::string> args);
		void topic_cmd(Client &client, std::vector<std::string> args);
		void mode_cmd(Client &client, std::vector<std::string> args);
		void invite_cmd(Client &client, std::vector<std::string> args);
		void kick_cmd(Client &client, std::vector<std::string> args);

};

#endif
