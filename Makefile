NAME = webserv

# Flags
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 # -g

# Directories
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj

# Files
SRC =	main.cpp			\
	Server.cpp			\
	Location.cpp			\
	parsing/configFile.cpp		\
	parsing/utils.cpp		\

OBJ = $(SRC:%.cpp=$(OBJ_DIR)/%.o)

# Colors
RESET			= "\033[0m"
GREEN			= "\033[32m"
RED			= "\033[91m"
YELLOW			= "\033[93m"


# Rules
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) -I $(INC_DIR)
	@echo $(GREEN)"--- EXECUTABLE ./$(NAME) is ready ---"$(RESET)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)
	@echo $(RED)"--- EXECUTABLE ./$(NAME) is removed ---"$(RESET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(INC_DIR)

re: fclean all

.PHONY: all clean fclean re
