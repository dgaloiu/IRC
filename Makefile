_SRCS = server.cpp main.cpp channel.cpp nick.cpp user.cpp pass.cpp quit.cpp signal.cpp chanmode.cpp replies.cpp privmsg.cpp join.cpp part.cpp ping.cpp  pong.cpp topic.cpp mode.cpp invite.cpp kick.cpp

SRCS = $(patsubst %.cpp, srcs/%.cpp, $(_SRCS))
OBJS = $(patsubst %.cpp, bin/%.o, $(_SRCS))

INCS = -I./incs
NAME = ircserv

CXX = clang++
CXXFLAGS = -Wall -Wextra -Werror -Wpedantic -Wvla -std=c++98 -MMD -MP

all: $(NAME)


$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCS) $^ -o $@

bin/%.o: srcs/%.cpp
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -rf bin
	rm -f $(NAME)

re: fclean all

.PHONY: all bonus clean fclean re