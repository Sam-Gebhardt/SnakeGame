CFLAGS= -W -Wall -g
programs=snake
LDFLAGS=-lncurses

snake: snake.o HashSet.o
	gcc -o snake $^ -lncurses

snake.o: snake.c
HashSet.o: HashSet.c

clean:
	rm -f snake.o snake HashSet.o
