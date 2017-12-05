CC = gcc
CFLAGS = -g3 -O0 -lm -Wall

all: MATH

MATH: ./obj/math.o
	$(CC) ./obj/math.o -o ./bin/math $(CFLAGS)

./obj/math.o: ./src/math.c
	$(CC) -c ./src/math.c -o ./obj/math.o $(CFLAGS)

clean:
	rm ./obj/*.o
