/**
 * C-Project von Gruppe 37
 *
 * main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"

#include "memory.h"
#include "globals.h"
#include "map.h"
#include "map_loader.h"
#include "output_buffer.h"
#include "sdl_output.h"
#include "action.h"
#include "main.h"
#include "spawn.h"

static Map *map;
static BufferTile *buf;

int main(int argc, char *argv[]){
	/*SDL anmachen*/
	if(SDL_Init(SDL_INIT_VIDEO))
		return EXIT_FAILURE;
	SDL_EnableKeyRepeat(200, 50);
	/*Karte laden*/
	if(argc==2)
		map=load_map(argv[1]);
	else{
		fprintf(stderr,"Kartennamen angeben\n");
		return EXIT_FAILURE;
	}
	if(map == NULL) {
		fprintf(stderr,"Fehler beim Laden der Karte\n");
		return EXIT_FAILURE;
	}
	map->msg_hist=ex_calloc(MESSAGE_STREAM_LIMIT, sizeof(char *));
	Spawn *player=get_player_spawn(map);
	if(player==NULL){
		fprintf(stderr, "Kein Spieler auf der Karte\n");
		return EXIT_FAILURE;
	}
	/*Wenn was im Inventar ist, anzeigen*/
	if(player->inventory_size)
		update_item(player->inventory[0]->name);
	/*Map zeichnen*/
	int num_tiles = OUTPUT_IN_GLYPHS_X*OUTPUT_IN_GLYPHS_Y, i;
	
	/* Ausgabepuffer initialisieren */
	buf = (BufferTile*)ex_malloc(sizeof(BufferTile) * num_tiles);
	for(i = 0; i < num_tiles; ++i) {
		BufferTile bt = {' ', 0x00000000};
		buf[i] = bt;
	}
	
	output_init(OUTPUT_IN_GLYPHS_X, OUTPUT_IN_GLYPHS_Y, map->name);
	explore_area(player, map);
	create_output_buffer(map, buf, num_tiles);
	output_draw(buf, num_tiles);
	
	/*Eingabeloop*/
	SDL_Event event;
	int quit=0;
	while(SDL_WaitEvent(&event)){
		if(event.type == SDL_KEYDOWN) {
			/*bei Escape beenden*/
			if(event.key.keysym.sym == SDLK_ESCAPE){
				quit=1;
				break;
			}
			process_event(&event, map);
			create_output_buffer(map, buf, num_tiles);
			output_draw(buf, num_tiles);
		} else if(event.type == SDL_QUIT) {
			quit=1;
			break;
		}
		SDL_Delay(1);
		/*Affe tot => Klappe zu*/
		if(player->hp<=0){
			game_over(0);
			break;
		}
		/* Ende erreicht */
		if(map->finished) {
			game_over(1);
			break;
		}
	}
	if(!quit){
		while(SDL_WaitEvent(&event)){
			/*bei Escape beenden*/
			if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
				break;
			}
			SDL_Delay(1);
		}
	}
	free(buf);
	flush_map(map);
	
	output_close();
	SDL_Quit();
	return EXIT_SUCCESS;
}

/*--------------------------------------------------------------------------*/
void create_output_buffer(Map* map, BufferTile* buf, int size) {
	/* Spieler in die Mitte, seine Position als Versatz benutzen */
	int i, j, translated_x, translated_y, center_x, center_y;
	Spawn* spawn = get_player_spawn(map);
	if(spawn == NULL) {
		fprintf(stderr, "Keine Spielfigur vorhanden!");
		exit(1);
	}
	clear_output_buffer(buf, size);

	center_x = OUTPUT_IN_GLYPHS_X / 2 + 1; center_y = OUTPUT_IN_GLYPHS_Y / 2 + 1;
	translated_x = center_x - spawn->x; translated_y = center_y - spawn->y;

	j = 0;
	for(i = 0; i < size; ++i) {
		unsigned int current_x, current_y;
		if(i != 0 && (i % (OUTPUT_IN_GLYPHS_X)) == 0) {
			++j;
		}
		current_y = j;
		current_x = i % OUTPUT_IN_GLYPHS_X;
		/* unteres Renderende erreicht */
		if(current_y == OUTPUT_IN_GLYPHS_Y) {
			break;
		}
		/* Hier Kartenteil? */
		if(translated_x <= (int)current_x && translated_y <= (int)current_y && current_x < (translated_x + map->x) && current_y < (translated_y + map->y)) {
			render_tile(&buf[i], &map->tiles[(current_y - translated_y) * map->x + (current_x - translated_x)], map);
		}
	}
}

/*--------------------------------------------------------------------------*/
 void clear_output_buffer(BufferTile* buf, int num) {
	int i = 0;
	for(; i < num; ++i) {
		buf[i].glyph = ' ';
		buf[i].color = 0x00000000;
	}
 }
