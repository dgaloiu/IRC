#include "Irc.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Commands.hpp"

void operatorChannelMode(Server *server, mode_struct mode_infos, int const client_fd, std::string str)
{
	std::map<const int, Client>::iterator it_client = server->getClients().find(client_fd);
	std::map<std::string, Channel>::iterator it_channel_target = server->getChannels().find(mode_infos.target);

	if (mode_infos.params.empty() == true)
		return;

	// checker que le param est un user IRC,puis user du channel
	std::map<const int, Client>::iterator it_user_target = server->getClients().begin();
	while (it_user_target != server->getClients().end())
	{
		if (it_user_target->second.getNickname() == mode_infos.params)
			break;
		it_user_target++;
	}
	if (it_user_target == server->getClients().end())
	{
		appendClientBuffer(server, client_fd, ERR_NOSUCHNICK(it_client->second.getNickname(), mode_infos.params));
		appendClientBuffer(server, client_fd, ERR_USERNOTINCHANNEL(it_client->second.getNickname(), mode_infos.params, mode_infos.target));
		return;
	}
	if (it_channel_target->second.getClientList().find(mode_infos.params) == it_channel_target->second.getClientList().end())
	{
		appendClientBuffer(server, client_fd, ERR_USERNOTINCHANNEL(it_client->second.getNickname(), mode_infos.params, mode_infos.target));
		return;
	}

	// Checker si param est dans la liste des operators du channel
	std::vector<std::string>::iterator it;
	for (it = it_channel_target->second.getOperators().begin(); it != it_channel_target->second.getOperators().end(); it++)
	{
		if (*it == mode_infos.params)
			break;
	}
	// checker si c'est pour rajouter ou enlever le mode
	if (str[0] == '+')
	{
		if (it != it_channel_target->second.getOperators().end()) // si param est deja operator
			return;

		// add au operator et send la reply en broadcast
		it_channel_target->second.getOperators().push_back(mode_infos.params);
		broadcastToAllChannelMembers(server, it_channel_target->second, MODE_CHANNELMSGWITHPARAM(mode_infos.target, "+o", mode_infos.params));
	}
	else // si c'est '-'
	{
		if (it == it_channel_target->second.getOperators().end()) // si param n'est pas operator
			return;
		// retirer du vector et send la reply en broadcast
		it_channel_target->second.getOperators().erase(it);
		broadcastToAllChannelMembers(server, it_channel_target->second, MODE_CHANNELMSGWITHPARAM(mode_infos.target, "-o", mode_infos.params));
	}
	// std::cout << "check operators du channel :" << std::endl;
	// for (std::vector<std::string>::iterator it = it_channel_target->second.getOperators().begin(); it != it_channel_target->second.getOperators().end(); it++)
	// 	std::cout << *it << std::endl;
}