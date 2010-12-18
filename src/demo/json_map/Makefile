CC = gcc
CFLAGS = -O2

mingw32: main.o json_parser.o
	$(CC) $(CFLAGS) -o demo.exe main.o json_parser.o
	
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
	
json_parser.o: main.c
	$(CC) $(CFLAGS) -c json_parser.c