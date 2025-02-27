# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mguerra <mguerra@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/19 10:42:24 by mguerra           #+#    #+#              #
#    Updated: 2023/08/19 11:53:41 by mguerra          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= ircserv

SRCS		= main.cpp server.cpp parsing.cpp utils.cpp \
				class/Client.cpp	\
				class/Channel.cpp	\
				class/Server.cpp	\
				commands/bot.cpp 	\
				commands/invite.cpp	\
				commands/join.cpp 	\
				commands/kick.cpp 	\
				commands/kill.cpp	\
				commands/list.cpp 	\
				commands/mode.cpp	\
				commands/motd.cpp 	\
				commands/names.cpp	\
				commands/nick.cpp	\
				commands/notice.cpp	\
				commands/oper.cpp	\
				commands/part.cpp	\
				commands/pass.cpp	\
				commands/ping.cpp 	\
				commands/privmsg.cpp	\
				commands/quit.cpp   \
				commands/topic.cpp	\
				commands/user.cpp 	\
				commands/modes/ban.cpp 		\
				commands/modes/key.cpp 		\
				commands/modes/limit.cpp 	\
				commands/modes/operator.cpp \
				commands/modes/invite.cpp 	\
				commands/modes/topic.cpp 	\

DIR_SRCS	= srcs/

DIR_OBJS	= objs/

OBJS		= ${SRCS:%.cpp=${DIR_OBJS}%.o}

CXX			= c++

DEP			= ${OBJS:%.o=%.d}

CPPFLAGS	= -Wall -Wextra -Werror -std=c++98 -c -I includes/ 
DEBUGFLAGS 	= -MMD -MP -g3 -D_GLIBCXX_DEBUG
RM 			= rm -f

all:	${NAME}

${NAME} : ${OBJS}
	${CXX} $^ -o $@

${OBJS} : ${DIR_OBJS}%.o: ${DIR_SRCS}%.cpp
	mkdir -p ${@D}
	${CXX} ${CPPFLAGS} $< -o $@
-include ${DEP}

clean:
	${RM} -r ${DIR_OBJS}

fclean: clean
	${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re