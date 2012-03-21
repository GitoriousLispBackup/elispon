CC = gcc
CFLAGS = -std=c99 -Wall -pedantic -g
LDFLAGS = 
DEBUG =
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
BIN = elispon

src/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

clean::
	rm -f $(BIN) $(OBJ)
