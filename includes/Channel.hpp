#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include "Irc.hpp"
#include "Client.hpp"

class Client;

class Channel
{
private:
	std::map<std::string, Client> _clientList;
	std::vector<std::string> _kicked_users;
	std::vector<std::string> _banned_users;
	std::vector<std::string> _invited_users;
	std::vector<std::string> _operators;
	std::vector<std::string> _voiced_users;
	std::string _name;
	std::string _operatorPassword;
	std::string _topic;
	std::string _mode;
	std::string _channel_password;
	int _capacity_limit;

public:
	Channel(std::string const &name);
	~Channel();

	bool doesClientExist(std::string &clientName);
	bool isBanned(std::string &banned_name);
	bool isInvited(std::string &invited_name);
	bool isOperator(std::string &operatorName);
	bool isVoiced(std::string &voiced_name);
	int &getCapacityLimit();
	std::map<std::string, Client> &getClientList();
	std::string &getChannelPassword();
	std::string &getMode();
	std::string &getName();
	std::string &getTopic();
	std::vector<std::string> &getBannedUsers();
	std::vector<std::string> &getKickedUsers();
	std::vector<std::string> &getOperators();
	void addClientToChannel(Client &client);
	void addFirstOperator(std::string operatorName);
	void addMode(std::string const mode);
	void addToBanned(std::string &banned_name);
	void addToKicked(std::string &banned_name);
	void addToVoiced(std::string &voiced_name);
	void removeChannelPassword();
	void removeClientFromChannel(std::string &clientName);
	void removeFromBanned(std::string &banned_name);
	void removeFromVoiced(std::string &voiced_name);
	void removeMode(std::string const mode);
	void removeOperator(std::string operatoName);
	void setCapacityLimit(int limit);
	void setChannelPassword(std::string password);
	void setTopic(std::string &newTopic);
};

#endif