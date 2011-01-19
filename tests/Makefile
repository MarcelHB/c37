CC = gcc
CFLAGS = -std=c89 -pedantic -Wall -Wextra -O2 -g
SRC = ../src/
TEST_MAP_LOADER_OBJECTS = test_map_loader.o JSON_Parser.o memory.o spawn.o tile.o map_loader.o
.SUFFIXES: .c

test_map_loader_win: $(TEST_MAP_LOADER_OBJECTS)
	$(CC) $(CFLAGS) $(TEST_MAP_LOADER_OBJECTS) -o test_map_loader.exe 
test_map_loader: $(TEST_MAP_LOADER_OBJECTS)
	$(CC) $(CFLAGS) $(TEST_MAP_LOADER_OBJECTS) -o test_map_loader

test_map_loader.o: test_map_loader.c
	$(CC) $(CFLAGS) -c test_map_loader.c
JSON_Parser.o: $(SRC)JSON_Parser.c
	$(CC) $(CFLAGS) -c $(SRC)JSON_Parser.c
memory.o: $(SRC)memory.c
	$(CC) $(CFLAGS) -c $(SRC)memory.c
spawn.o: $(SRC)spawn.c
	$(CC) $(CFLAGS) -c $(SRC)spawn.c
tile.o: $(SRC)tile.c
	$(CC) $(CFLAGS) -c $(SRC)tile.c
map_loader.o: $(SRC)map_loader.c
	$(CC) $(CFLAGS) -c $(SRC)map_loader.c

	