CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c99 -g 
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
BIN = crisp-repl

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -ledit -o $@

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build $(BIN)

