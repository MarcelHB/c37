CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -O0 -g
SRC = ../src/
TEST_MAP_LOADER_OBJECTS = test_map_loader.o JSON_parser.o memory.o spawn.o tile.o map_loader.o item.o
TEST_LOGIC_OBJECTS = test_logic.o JSON_parser.o memory.o spawn.o tile.o map_loader.o item.o map.o action.o
.SUFFIXES: .c

all: test_logic test_map_loader
test_logic: $(TEST_LOGIC_OBJECTS)
	$(CC) $(CFLAGS) $(TEST_LOGIC_OBJECTS) -o test_logic -lm
test_map_loader: $(TEST_MAP_LOADER_OBJECTS)
	$(CC) $(CFLAGS) $(TEST_MAP_LOADER_OBJECTS) -o test_map_loader -lm
clean:
	rm -f *.o
	rm -f *.exe

test_logic.o: test_logic.c
	$(CC) $(CFLAGS) -c test_logic.c
test_map_loader.o: test_map_loader.c
	$(CC) $(CFLAGS) -c test_map_loader.c
JSON_parser.o: $(SRC)JSON_parser.c
	$(CC) $(CFLAGS) -c $(SRC)JSON_parser.c
memory.o: $(SRC)memory.c
	$(CC) $(CFLAGS) -c $(SRC)memory.c
spawn.o: $(SRC)spawn.c
	$(CC) $(CFLAGS) -c $(SRC)spawn.c
tile.o: $(SRC)tile.c
	$(CC) $(CFLAGS) -c $(SRC)tile.c
map_loader.o: $(SRC)map_loader.c
	$(CC) $(CFLAGS) -c $(SRC)map_loader.c
map.o: $(SRC)map.c
	$(CC) $(CFLAGS) -c $(SRC)map.c
item.o: $(SRC)item.c
	$(CC) $(CFLAGS) -c $(SRC)item.c
action.o: $(SRC)action.c
	$(CC) $(CFLAGS) -c $(SRC)action.c

	