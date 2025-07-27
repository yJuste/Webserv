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
	debug.cpp				\

OBJ = $(SRC:%.cpp=$(OBJ_DIR)/%.o)

# Colors
RESET			= "\033[0m"
GREEN			= "\033[32m"
RED			= "\033[91m"
YELLOW			= "\033[93m"
VIOLETC			= "\033[94m"
VIOLETF			= "\033[95m"


# Rules
all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME) -I $(INC_DIR)
	@echo $(VIOLETF)"➤ '$(NAME)' is now ready."$(RESET)

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)
	@echo $(RED)"➤ Every files and $(NAME) are removed."$(RESET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo $(VIOLETC)"Compiling: $<"$(RESET)
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(INC_DIR)

re: fclean all

.PHONY: all clean fclean re
