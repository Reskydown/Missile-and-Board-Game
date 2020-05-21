CC = gcc
CFLAGS = -std=c89 -Wall -pedantic -ansi -Werror -g
OBJ = missile.o ship.o game.o main.o
EXEC = battleships

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)
	
ship.o: ship.c ship.h
	$(CC) -c ship.c $(CFLAGS)

missile.o: missile.c missile.h
	$(CC) -c missile.c $(CFLAGS)

game.o: game.c game.h
	$(CC) -c game.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)

