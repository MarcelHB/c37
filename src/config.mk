CFLAGS = -std=c99 -Wall -Wextra -pedantic -O0 -g $(shell sdl-config --cflags)
LDFLAGS = -g $(shell sdl-config --libs)

CC = gcc
