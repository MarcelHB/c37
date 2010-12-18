CC = gcc
CFLAGS = -O2

mingw32: main.o json_parser.o
	$(CC) $(CFLAGS) -g -o demo.exe main.o json_parser.o
	
main.o: main.c
	$(CC) $(CFLAGS) -g -c main.c
	
json_parser.o: main.c
	$(CC) $(CFLAGS) -g -c json_parser.c