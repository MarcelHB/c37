include config.mk

SRC = main.c JSON_parser.c map_loader.c memory.c spawn.c tile.c sdl_output.c map.c item.c
OBJ = ${SRC:.c=.o}

all: c37

.c.o:
	${CC} ${CFLAGS} -c $<

${OBJ}: config.mk globals.h item.h JSON_parser.h main.h map_loader.h memory.h output_buffer.h sdl_output.h spawn.h text_output.h tile.h

c37: ${OBJ}
	${CC} -o ../$@ ${OBJ} ${LDFLAGS}

clean:
	rm -f c37 ${OBJ}

.PHONY: all clean
