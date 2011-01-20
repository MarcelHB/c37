#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include "globals.h"
#include "map.h"
#include "map_loader.h"
#include "output_buffer.h"
#include "text_output.h"
#include "action.h"

int main (int argc, char *argv[]) {
	if(SDL_Init(SDL_INIT_VIDEO))
		return EXIT_FAILURE;
	Map *map;
	if(argc==2)
		map=load_map(argv[1]);
	else{
		fprintf(stderr,"Kartennamen angeben");
		return EXIT_FAILURE;
	}
	int num_tiles=OUTPUT_IN_GLYPHS_X*OUTPUT_IN_GLYPHS_Y;
	output_init(OUTPUT_IN_GLYPHS_X, OUTPUT_IN_GLYPHS_Y);
	BufferTile *buf;
	create_output_buffer(map, buf, num_tiles);
	output_draw(buf, num_tiles);
	SDL_Event *event;
	while(1){
		if(!SDL_WaitEvent(event))
			return EXIT_FAILURE;
		if(event->key.keysym.sym==SDLK_ESCAPE)
			break;
		process_event(event, map);
		create_output_buffer(map, buf, num_tiles);
		output_draw(buf, num_tiles);
	}
	output_clear();
	output_close();
	SDL_Quit();
	return EXIT_SUCCESS;
}
