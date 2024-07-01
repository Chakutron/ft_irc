NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -g -std=c++98 

SRCS = src/main.cpp \
	src/IRCServer.cpp \
	src/Client.cpp \
	src/Channel.cpp \
	src/Command.cpp \
	src/NetworkManager.cpp \
	src/ConfigParser.cpp \
	src/IRCCodes.cpp \
	src/Logger.cpp

OBJS = $(SRCS:.cpp=.o)

GREEN = \e[0;32m
YELLOW = \e[0;33m
NC = \e[0m

HEADERS = inc/IRCServer.hpp \
		inc/Client.hpp \
		inc/Channel.hpp \
		inc/Command.hpp \
		inc/NetworkManager.hpp \
		inc/ConfigParser.hpp \
		inc/Logger.hpp \
		inc/IRCCodes.hpp \
		inc/IRCException.hpp

all: $(NAME)

$(NAME): compiling $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "]\n✅ ${GREEN}Done!${NC}"

%.o: %.cpp $(HEADERS)
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo -n "${YELLOW}*${NC}"

compiling:
		@echo -n "Generating ${GREEN}${NAME}${NC} executable: ["

clean:
	@echo "❌ Deleting object files.."
	@rm -f $(OBJS)

fclean: clean
	@echo "❌ Deleting executable files.."
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re compiling