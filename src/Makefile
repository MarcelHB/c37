include config.mk

SRC = action.c main.c item.c JSON_parser.c map.c map_loader.c memory.c spawn.c sdl_output.c tile.c
OBJ = ${SRC:.c=.o}

all: c37

.c.o:
	${CC} ${CFLAGS} -c $<

${OBJ}: config.mk action.h globals.h item.h JSON_parser.h main.h map.h map_loader.h memory.h output_buffer.h sdl_output.h spawn.h text_output.h tile.h

c37: ${OBJ}
	${CC} -o ../$@ ${OBJ} ${LDFLAGS}

clean:
	rm -f ../c37 ${OBJ}

.PHONY: all clean
