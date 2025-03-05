# ft_irc

An IRC (Internet Relay Chat) server implementation based on RFC standards. This project provides a functional IRC server where users can connect, authenticate, join channels, and communicate with each other.

## Overview

This IRC server implementation follows the IRC protocol as defined in:
- RFC 1459 - Internet Relay Chat Protocol
- RFC 2812 - Internet Relay Chat: Client Protocol (most useful)
- RFC 2813 - Internet Relay Chat: Server Protocol

## Features

- User authentication and registration
- Channel operations (join, part, topic management)
- Private messaging between users
- Channel operator privileges
- Full compliance with IRC protocol standards

## Requirements

- C++ compiler with C++98 support
- Make or CMake
- POSIX-compliant operating system

## Building

```
git clone https://github.com/dgaloiu/ft_irc.git
cd ft_irc
make
```

## Be able to authenticate, set a nickname, a username messages using your reference client

### set pass:
	Replies: ERR_NEEDMOREPARAMS, ERR_ALREADYREGISTRED 
	
	ex: PASS secretpasswordhere
### set nickname:
	Replies: ERR_NONICKNAMEGIVEN, ERR_ERRONEUSNICKNAME, ERR_NICKNAMEINUSE, ERR_NICKCOLLISION, ERR_UNAVAILRESOURCE, ERR_RESTRICTED

	ex: NICK user
### set username:
	Replies: ERR_NEEDMOREPARAMS, ERR_ALREADYREGISTRED

	ex: USER user user localhost :user

### authentificate:
A "PASS" command is not required for either client or server connection to be registered, but it must precede the server message or the latter of the NICK/USER combination
1. Pass message
2. Nick message
3. User message

## Join a channel, send and receive private messages

* JOIN - Allows users to enter a channel
	```
	Syntax: JOIN <channel>{,<channel>} [<key>{,<key>}]
	Example: JOIN #testchannel password123
	
	Replies:
				ERR_NEEDMOREPARAMS	ERR_BANNEDFROMCHAN
				ERR_INVITEONLYCHAN	ERR_BADCHANNELKEY
				ERR_CHANNELISFULL	ERR_BADCHANMASK
				ERR_NOSUCHCHANNEL	ERR_TOOMANYCHANNELS
				ERR_TOOMANYTARGETS	ERR_UNAVAILRESOURCE
				RPL_TOPIC
	```

* PART - Removes the client from the specified channels
	```
	Syntax: PART <channel>{,<channel>} [<reason>]
	Example: PART #channel "Goodbye for now"
	
	Replies:
				ERR_NEEDMOREPARAMS
				ERR_NOSUCHCHANNEL
				ERR_NOTONCHANNEL
	```

* MODE - Changes channel or user modes
	```
	Syntax: MODE <target> <modes> [<mode-parameters>]
	Example: MODE #channel +o username
	
	Replies:	ERR_NEEDMOREPARAMS		ERR_KEYSET
				ERR_NOCHANMODES			ERR_CHANOPRIVSNEEDED
				ERR_USERNOTINCHANNEL	ERR_UNKNOWNMODE
				RPL_CHANNELMODEIS
				RPL_BANLIST				RPL_ENDOFBANLIST
				RPL_EXCEPTLIST			RPL_ENDOFEXCEPTLIST
				RPL_INVITELIST			RPL_ENDOFINVITELIST
				RPL_UNIQOPIS
	```

* KICK - Forcibly removes a user from a channel
	```
	Syntax: KICK <channel> <user> [<comment>]
	Example: KICK #channel baduser :Inappropriate language
	
	Replies:
				ERR_NEEDMOREPARAMS		ERR_NOSUCHCHANNEL
				ERR_BADCHANMASK			ERR_CHANOPRIVSNEEDED
				ERR_USERNOTINCHANNEL	ERR_NOTONCHANNEL
	```

* QUIT - Terminates the client's connection to the server
	```
	Syntax: QUIT [<quit message>]
	Example: QUIT :Gone to lunch
	
	Replies:
			None
	```

* PRIVMSG/NOTICE - Sends private messages to users or channels
	```
	Syntax: PRIVMSG <receiver>{,<receiver>} :<text>
	Example: PRIVMSG nickname :Hello there!
	Example: PRIVMSG #channel :Hello everyone!
	
	Replies:
			ERR_NORECIPIENT			ERR_NOTEXTTOSEND
			ERR_CANNOTSENDTOCHAN	ERR_NOTOPLEVEL
			ERR_WILDTOPLEVEL		ERR_TOOMANYTARGETS
			ERR_NOSUCHNICK			RPL_AWAY
	```

* TOPIC - Views or sets a channel's topic
	```
	Syntax: TOPIC <channel> [:<topic>]
	Example: TOPIC #channel :New channel topic
	
	Replies:
			ERR_NEEDMOREPARAMS		ERR_NOTONCHANNEL
			RPL_NOTOPIC				RPL_TOPIC
			ERR_CHANOPRIVSNEEDED	ERR_NOCHANMODES
	```

* INVITE - Invites a user to a channel
	```
	Syntax: INVITE <nickname> <channel>
	Example: INVITE friend #mychannel
	
	Replies:
			ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
			ERR_NOTONCHANNEL                ERR_USERONCHANNEL
			ERR_CHANOPRIVSNEEDED
			RPL_INVITING                    RPL_AWAY
	```

* OPER - Requests operator privileges
	```
	Syntax: OPER <username> <password>
	Example: OPER admin secretpass
	
	Replies:
			ERR_NEEDMOREPARAMS              RPL_YOUREOPER
			ERR_NOOPERHOST                  ERR_PASSWDMISMATCH
	```