CC=clang
LIBS=-lSDL2 -lm
CFLAGS=-Wall -pedantic
SRC=./src/upng.c ./src/textures.c ./src/main.c

build:
	$(CC) $(CFLAGS) -std=c99 $(SRC) $(LIBS) -o raycast

run:
	./raycast

clean:
	rm ./raycast
