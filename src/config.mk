CFLAGS = -std=c99 -Wall -Wextra -pedantic -O0 -g $(shell sdl-config --cflags)
LDFLAGS = -g $(shell sdl-config --libs) -lSDL_ttf

CC = gcc
