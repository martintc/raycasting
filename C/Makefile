CC=clang
LIBS=-lSDL2 -lm
CFLAGS=-Wall -pedantic

build:
	$(CC) $(CFLAGS) -std=c99 ./src/*.c $(LIBS) -o raycast

run:
	./raycast

clean:
	rm ./raycast