NAME = webserv

# Flags
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g

# Directories
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj

# Files
SRC =	main.cpp				\
	Server.cpp				\
	Location.cpp				\
	parsing/configure_file.cpp		\
	parsing/init_server.cpp			\
	parsing/init_location.cpp		\
	parsing/utils.cpp			\

OBJ = $(SRC:%.cpp=$(OBJ_DIR)/%.o)

# Colors
RESET		= "\033[0m"
BLUE		= "\033[38;2;0;128;157m"
BEIGE		= "\033[38;2;252;248;221m"
YELLOW		= "\033[38;2;255;215;0m"
BROWN		= "\033[38;2;211;175;55m"


# Rules
all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME) -I $(INC_DIR)
	@echo $(BLUE) "➤ '$(NAME)' is now ready." $(RESET)

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)
	@echo $(YELLOW) "➤ Every files and $(NAME) are removed." $(RESET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo $(BROWN) "Compiling ➤ $<" $(RESET)
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(INC_DIR)

re: fclean all

.PHONY: all clean fclean re
