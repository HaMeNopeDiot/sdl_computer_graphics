SRC_DIR = src
OBJ_DIR = objs
TESTS_DIR = tests
TEST_MFS := $(wildcard $(TESTS_DIR)/*/Makefile)
TESTS := $(foreach mkf, $(TEST_MFS), $(notdir $(patsubst %/,%, $(dir $(mkf)))))

SOURCES = $(wildcard $(SRC_DIR)/*.cpp $(TESTS_DIR)/*.cpp)
OBJECTS = $(foreach src_file, $(SOURCES), $(OBJ_DIR)/$(notdir $(src_file:.cpp=.o)))

BASE = engine

CC = clang++

CFLAGS = -Og -g3 -Wall -I$(SRC_DIR)
LDFLAGS = -lSDL2main -lSDL2  # Добавляем флаги для линковки с SDL2

OBJ_DIR_EX := $(shell mkdir -p $(OBJ_DIR) && echo $(OBJ_DIR))

all: clean $(BASE)

$(BASE): .depend $(OBJECTS)
	$(CC) $(filter-out .depend,$^) -Og -g -o "$@" $(LDFLAGS)  # Добавляем линковочные флаги

.depend: $(SOURCES)
	rm -f "$@"
	$(CC) $(CFLAGS) -MM $^ -MF "$@"

include .depend

$(OBJ_DIR_EX)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR_EX)/%.o: $(TESTS_DIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

CLEANFILES = $(BASE) .depend $(OBJ_DIR)/*.o

.PHONY: clean
clean:
	rm -f $(CLEANFILES)