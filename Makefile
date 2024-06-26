# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mchiboub <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/07 15:15:55 by mchiboub          #+#    #+#              #
#    Updated: 2023/11/06 17:16:28 by mchiboub         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -g -std=c++98

SRCS = 	src/Server.cpp			\
		src/runServ.cpp			\
		src/ServerUtils.cpp		\
		src/handlePass.cpp		\
		src/handleUser.cpp		\
		src/handleNick.cpp		\
		src/handlePing.cpp		\
		src/handleJoin.cpp		\
		src/handleQuit.cpp		\
		src/handleKick.cpp		\
		src/handleInvite.cpp	\
		src/handleTopic.cpp		\
		src/handleMode.cpp		\
		src/handlePrivMsg.cpp	\
		src/main.cpp

OBJS = ${SRCS:.cpp=.o}

GREEN = \e[0;32m
YELLOW = \e[0;33m
NC = \e[0m

all: ${NAME}

.cpp.o:
		@${CC} ${CFLAGS} -c $< -o ${<:.cpp=.o}
		@echo -n "${YELLOW}*${NC}"

${NAME}: compiling ${OBJS}
		@${CC} ${CFLAGS} -o ${NAME} ${OBJS}
		@echo "]\n✅ ${GREEN}Done!${NC}"

compiling:
		@echo -n "Generating ${GREEN}${NAME}${NC} executable: ["

clean:
		@echo "❌ Deleting object files.."
		@rm -f ${OBJS}

fclean: clean
		@echo "❌ Deleting executable files.."
		@rm -f ${NAME}

re: fclean all