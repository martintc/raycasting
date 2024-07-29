CC=clang
LIBS=-lSDL2 -lm
CFLAGS=-Wall -pedantic
SRC=./src/upng.c ./src/player.c ./src/ray.c ./src/textures.c ./src/graphics.c ./src/map.c ./src/wall.c ./src/main.c

build:
	$(CC) $(CFLAGS) -std=c99 $(SRC) $(LIBS) -o raycast

run:
	./raycast

clean:
	rm ./raycast
