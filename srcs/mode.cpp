#include <sstream>
#include "server.hpp"

void IRCServer::mode_cmd(Client &client, std::vector<std::string> args)
{
	/*
	RESPONSES:
		ERR_NEEDMOREPARAMS              ERR_KEYSET
		ERR_NOCHANMODES (not todo)      ERR_CHANOPRIVSNEEDED
		ERR_USERNOTINCHANNEL            ERR_UNKNOWNMODE
		RPL_CHANNELMODEIS
		RPL_BANLIST (not todo)                     RPL_ENDOFBANLIST (not todo)
		RPL_EXCEPTLIST (not todo)                 RPL_ENDOFEXCEPTLIST (not todo)
		RPL_INVITELIST                  RPL_ENDOFINVITELIST
		RPL_UNIQOPIS
	
	NOTES:
		- there is a maximum limit of three (3) changes per command for modes that take a parameter.
		- only first mode change seems to be to applied
	*/
	std::map<std::string, Channel>::iterator chan_it;
	std::map<std::string, u_int8_t>::iterator cli_it;

	if (args.size() < 1)
	{
		send_reply(client, ERR_NEEDMOREPARAMS, "MODE");
		return ;
	}

	if (is_nickname(args[0]))
	{
		send_reply(client, ERR_UNKNOWNCOMMAND, "(user)MODE"); // Cause Unimplemented
		return ;
	}
	chan_it = this->channels.find(args[0]);
	if (chan_it == this->channels.end())
	{
		this->send_reply(client, ERR_NOSUCHCHANNEL, args[0].c_str());
		return ;
	}

	if (args.size() == 1)
	{
		std::stringstream ssm;
		std::stringstream ssp;
		std::map<char, std::vector<std::string> > chm = chan_it->second.getModeFor(client);
		std::map<char, std::vector<std::string> >::iterator chit = chm.begin();
		ssm << '+';
		for ( ; chit!=chm.end(); ++chit)
		{
			ssm << chit->first;
		}
		for (chit = chm.begin(); chit != chm.end(); )
		{
			size_t j = 0;
			std::cout << "PARAM SIZE:" << chit->second.size() << std::endl;
			bool params_empty = chit->second.empty();
			for ( ; j < chit->second.size(); ++j)
			{
				ssp << chit->second[j];
				if (j + 1 < chit->second.size()) ssp << " ";
			}
			++chit;
			if (chit != chm.end() && params_empty != true && !chit->second.empty()) ssp << " ";
		}
		this->send_reply(client, RPL_CHANNELMODEIS, chan_it->first.c_str(), ssm.str().c_str(), ssp.str().c_str());
	}
	else
	{
		/*
			Follow IRSSI syntax: MODE <channel> [<mode> [<mode parameters>]]
		*/

		if (chan_it->second.getUserPriv(&client) < Operator)
		{
			this->send_reply(client, ERR_CHANOPRIVSNEEDED, args[0].c_str());
			return ;
		}

		std::vector<std::pair<ChanMode*, char> > chm_status;
		std::vector<std::vector<std::string> > params;
		char state;
		size_t jj = 0;

		for (size_t k = 1; k < args.size(); )
		{
			state = '+';
			for (size_t i = 0; i < args[k].size(); ++i)
			{
				ChanMode* mod;
				if (args[k][i] == '+' || args[k][i] == '-')
				{
					state = args[k][i];
					++i;
				}
				try
				{
					mod = this->chanmodes.at(args[k][i]);
					chm_status.push_back(std::make_pair(mod, state));
				}
				catch(std::out_of_range &e)
				{
					std::string uknm(1, args[k][i]);
					this->send_reply(client, ERR_UNKNOWNMODE, uknm.c_str());
				}
			}
			++k;
			for ( ; jj < chm_status.size(); ++jj)
			{
				std::vector<std::string> pars;
				size_t nb_params = chm_status[jj].first->getNbParam(chm_status[jj].second);
				std::cout << nb_params << std::endl;
				if (k < args.size())
				{
					for (size_t n = 0; n < nb_params && k + n < args.size(); ++n)
						pars.push_back(args[k + n]);
				}
				params.push_back(pars);
				/*
				std::cout << "PARAMS: " << std::endl;
				for (size_t k = 0; k < pars.size(); ++k)
					std::cout << pars[k] << std::endl;
				*/
				k += nb_params;
			}
		}

		//Apply changed modes
		std::set<ChanMode*> chm_activ;
		int chm_err;
		std::stringstream ssm;
		std::stringstream ssp;
		std::string modeset_msg;
		for (size_t jj = 0; jj < chm_status.size() && chm_activ.size() < 3; ++jj)
		{
			if (params[jj].size() < chm_status[jj].first->getNbParam(chm_status[jj].second))
			{
				// if nb params not enough, ignore silently
				continue;
			}

			if (chm_activ.count(chm_status[jj].first) > 0)
				continue;
			chm_err = chm_status[jj].first->apply(*this, chan_it->second, client, chm_status[jj].second == '+', params[jj]);
			if (chm_err > 0)
			{
				chm_activ.insert(chm_status[jj].first);
				ssm << ' ' << chm_status[jj].second << chm_status[jj].first->getName();
				for (size_t i = 0; i < params[jj].size(); ++i)
				{
					ssp << ' ' << params[jj][i];
				}
			}
		}
		modeset_msg = "MODE " + chan_it->first + ssm.str() + ssp.str();
		chan_it->second.send_msg(client, modeset_msg);
		/*
		for (size_t k = 0; k < chmodes.size(); ++k)
			std::cout << chmodes[k] << std::endl;
		*/
	}

}
