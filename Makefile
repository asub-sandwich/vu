CC = gcc

CFLAGS = -Wall -Wextra -std=c99 -O2 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm

SRC = main.c
OBJ = $(SRC:.c=.o)
BIN = vu

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(BIN) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)
