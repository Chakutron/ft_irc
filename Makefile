# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: orauline <orauline@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/14 20:31:43 by orauline          #+#    #+#              #
#    Updated: 2024/05/07 14:56:53 by orauline         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


# Colors
RESET = \033[0m
RED = \033[0;91m
GREEN = \033[0;92m
YELLOW = \033[0;93m
BLUE = \033[0;94m

NAME = ircserv

CC = c++

CPPFLAGS = -Wall -Werror -Wextra -std=c++98

SRCS = *.cpp

all: $(NAME)

$(NAME):
	@$(CC) $(CPPFLAGS) $(SRCS) -o $(NAME)
	@echo "$(GREEN)$(NAME) compiled successfully ! 🎉 ! $(RESET)"

re: fclean all

clean:
	@echo "$(RED)Cleaning objects...🧹$(RESET)"
	@rm -f *.o
	@echo "$(GREEN)Done! ✅$(RESET)"

fclean: clean
	@echo  "$(RED)Cleaning all...🧽$(RESET)"
	@rm $(NAME)
	@echo "$(BLUE)Cleaning binaries...🧼$(RESET)"
	@echo "$(GREEN)Done! ✅$(RESET)"

.PHONY: all re clean fclean