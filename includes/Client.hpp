#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Irc.hpp"

class Client
{
	private:
		int				_client_fd;
		std::string		_readbuf;
		std::string		_sendbuf;
		bool			_to_deconnect;
		std::string		_nickname;
		std::string		_old_nickname;
		std::string		_username;
		std::string		_realname;
		std::string		_mode;
		bool			_connexion_password;
		bool			_registrationDone;
		bool			_welcomeSent;
		bool			_hasAllInfo;
		int				_nbInfo;
	
	public:
		Client(int client_fd);
		~Client();
		
		bool&			getConnexionPassword();
		bool&			getDeconnexionStatus();
		bool&			hasAllInfo();
		bool&			isRegistrationDone();
		bool&			isWelcomeSent();
		int				getClientFd()const;
		int				getNbInfo() const;
		int				is_valid()const;
		std::string		getRealname()const;
		std::string		getUsername()const;
		std::string&	getMode();
		std::string&	getNickname();
		std::string&	getOldNickname();
		std::string&	getReadBuffer();
		std::string&	getSendBuffer();
		void			addMode(std::string const mode);
		void			removeMode(std::string const mode);
		void			resetReadBuffer(std::string const &str);
		void			setConnexionPassword(bool boolean);
		void			setDeconnexionStatus(bool status);
		void			sethasAllInfo(bool boolean);
		void			setNbInfo(int n);
		void			setNickname(std::string const &nickname);
		void			setOldNickname(std::string const &nickname);
		void			setReadBuffer(std::string const &buf);
		void			setRealname(std::string const &realname);
		void			setRegistrationDone(bool boolean);
		void			setSendBuffer(std::string const &buf);
		void			setUsername(std::string const &username);
		void			setWelcomeSent(bool boolean);
};

#endif