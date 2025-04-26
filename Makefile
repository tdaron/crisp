CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -O3
LDFLAGS = -ledit
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
BIN = crisp


ifdef WITH_RAYLIB
    CFLAGS += -DWITH_RAYLIB
	LDFLAGS += -lraylib
endif


all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -ledit -o $@

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build $(BIN)


