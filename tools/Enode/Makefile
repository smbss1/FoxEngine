##
## EPITECH PROJECT, 2019
## Makefile
## File description:
## make an executable
##

CC = g++

EXECUTABLE	= enode
SRC_DIR = src

SRC = $(shell find $(SRC_DIR) -name '*.cpp')
OBJ = $(SRC:%.cpp=%.o)

CFLAGS += -std=c++17 -W -Wall -Wextra $(if $(DEBUG),-g3) $(if $(DEBUG),-DDEBUG)
LDFLAGS = -ldl -Llib -llexer
INC_FLAGS = -Iinclude -I lib/GenericLexer/include

all: lib_all bin/$(EXECUTABLE)

lib_all:
	@make -sC lib

lib_fclean:
	@make fclean -sC lib

run: all
	@./bin/$(EXECUTABLE) $(ARGS)

bin/$(EXECUTABLE): $(OBJ)
	@$(CC) -o $@ $(OBJ) $(CFLAGS) $(INC_FLAGS) $(LDFLAGS)

%.o: %.cpp
	@$(CC) $(INC_FLAGS) $(CFLAGS) -c $< -o $@
	@echo "\033[1;32mCompiled \033[1;37m'$<'\033[m"

.PHONY: clean
clean:
	@$(RM) -r $(OBJ)

.PHONY: fclean
fclean: lib_fclean clean
	@$(RM) -r $(BIN)/$(EXECUTABLE)
	@$(RM) -r vgcore*

.PHONY: re
re: fclean all

dll:
	@g++ -g3 -Wall -Wno-write-strings -g -fPIC -shared -I include -I lib/GenericLexer/include ./test.cpp -o test.so
