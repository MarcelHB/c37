CC = gcc
SDL_FLAGS = `sdl-config --cflags`
CFLAGS = -O2 $(SDL_FLAGS)
LIBS = `sdl-config --libs` -lSDL_ttf

mingw32: main.o
	$(CC) $(CFLAGS) -o demo.exe main.o $(LIBS)
	
main.o: main.c
	$(CC) $(CFLAGS) -c main.c