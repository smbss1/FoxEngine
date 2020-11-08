##
## EPITECH PROJECT, 2019
## Makefile
## File description:
## make an executable
##

CC = g++ -std=c++17

EXECUTABLE	= FoxEngine
SRC_DIR = src

MAIN_SRC = main.cpp
MAIN_OBJ = $(MAIN_SRC:%.cpp=%.o)
SRC = $(shell find $(SRC_DIR) -name '*.cpp')
OBJ = $(SRC:%.cpp=%.o)

LDFLAGS = -Llib/Foxely -lfoxely -Llib/Foxely/lib -llexer -pthread
INC_FLAGS = -Iinclude -Ilib/Foxecs/include -Ilib/Foxely/include -Ilib/Foxely/lib/GenericLexer/include

ifeq ($(BUILD),debug)
# "Debug" build - no optimization, and debugging symbols
CFLAGS += -O0 -g3
CFLAGS += -DDEBUG -DDEBUG_TRACE_EXECUTION
CFLAGS += $(if $(DEBUG_TOKEN),-DDEBUG_TOKEN)
CFLAGS += $(if $(DEBUG_LOG_GC),-DDEBUG_LOG_GC)
else
# "Release" build - optimization, and no debug symbols
CFLAGS += -O3 -s -DNDEBUG
endif

ENODES = $(shell find Enodes -name '*.enode')

all: lib_all bin/$(EXECUTABLE)

generate_headers:
	@g++ -Wall -Wno-write-strings -g -fPIC -shared -I ./tools/Enode/include -I ./tools/Enode/lib/GenericLexer/include ./EngineCode.cpp -o generator.so
	@./tools/Enode/bin/enode -c ./generator.so $(ENODES)

lib_all:
	@make -sC lib

lib_clean:
	@make clean -sC lib

lib_fclean:
	@make fclean -sC lib

run: bin/$(EXECUTABLE)
	@./bin/$(EXECUTABLE) $(ARGS)

bin/$(EXECUTABLE): $(MAIN_OBJ) $(OBJ)
	@$(CC) -o $@ $(MAIN_OBJ) $(OBJ) $(CFLAGS) $(INC_FLAGS) $(LDFLAGS)

%.o: %.cpp
	@$(CC) $(INC_FLAGS) $(CFLAGS) -c $< -o $@
	@echo "\033[1;32mCompiled \033[1;37m'$<'\033[m"

.PHONY: clean
clean: lib_clean
	@$(RM) -r $(OBJ) $(MAIN_OBJ)

.PHONY: fclean
fclean: lib_fclean clean
	@$(RM) -r bin/$(EXECUTABLE)
	@$(RM) -r vgcore*

.PHONY: re
re: fclean all
