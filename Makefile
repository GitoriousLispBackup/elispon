CC = gcc
CFLAGS = -std=c99 -Wall -pedantic -g
LDFLAGS =
DEBUG =
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
BIN = elispon

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

src/primitive.o: $(wildcard src/primitives/*.c)

src/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean::
	rm -f $(BIN) $(OBJ)
