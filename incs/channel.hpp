#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include "client.hpp"
#include "chanmode.hpp"

enum user_privileges
{
	Normal = 0,
	Operator = 1
};

class ChanMode;

class Channel
{
	private:
		std::string name;
		std::string pass;
		std::string topic;
		size_t user_limit;
		std::map<Client*, user_privileges> users; // with op status
		std::set<Client*> invited;
		std::map<char, std::vector<std::string> > modes;
	public:
		Channel();
		Channel(std::string &name, std::string pass = "");
		/*
		Channel(Channel const &chan);
		Channel& operator=(Channel const &chan);
		*/
		~Channel();
		std::string const &getName() const;
		std::string const &getTopic() const;
		std::map<Client*, user_privileges> const &getUsers() const;
		int checkPass(std::string const &key) const;
		std::pair<char, std::vector<std::string> > getModeFor(Client &cli, char m);
		std::map<char, std::vector<std::string> > getModeFor(Client &cli, std::string mstr);
		std::map<char, std::vector<std::string> > getModeFor(Client &cli);
		bool is_mode_same_on_chan(ChanMode &chm, bool status, std::vector<std::string>& args);
		void setMode(char m, bool status, std::vector<std::string> params);
		void setPass(std::string const &pass);
		void setTopic(std::string const &topic);
		void setUserLimit(size_t n);
		user_privileges getUserPriv(Client *cli_ptr);
		void setUserPriv(Client *cli_ptr, user_privileges up);
		size_t getUserLimit();
		size_t getUserNumber();
		unsigned can_join_chan(Client &cli, std::string &key);
		void add_user(Client &cli, user_privileges up);
		void remove_user(Client &cli);
		void add_invitation(Client &cli);
		void remove_invitation(Client &cli);
		void send_msg(Client &src, std::string &msg, bool send_to_client = true);
		bool is_user_on(Client &cli);
		bool is_user_invited(Client &cli);
};

#endif
