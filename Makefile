NAME = ircserv

SRC = clients/clients.cpp \
	srcs/Server.cpp \
	main.cpp

OBJ = $(SRC:.CPP=.o)

C++ = c++

C++FLAGS = -Wall -Werror -Wextra -std=c++98

$(NAME): $(OBJ)
	$(C++) $(C++FLAGS) -o $(NAME) $(OBJ)

all: $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

