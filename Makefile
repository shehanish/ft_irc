# ========== Compiler and Flags ==========
NAME = ircserv

CXX = c++

CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -Iincludes

# ========== Directories ==========
SRC_DIR     = srcs
CLIENTS_DIR = clients
OBJ_DIR     = objs
INC_DIR     = includes

# ========== Source Files ==========
SRC = $(CLIENTS_DIR)/clients.cpp \
	$(SRC_DIR)/Server.cpp \
	main.cpp

# ========== Object Files ==========
OBJ         = $(SRC:%.cpp=$(OBJ_DIR)/%.o)

# ========== Rules ==========
all: $(NAME)

$(NAME): $(OBJ)
	@mkdir -p $(dir $(OBJ))
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

