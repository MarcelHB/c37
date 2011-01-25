/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL.h"

#include "memory.h"
#include "globals.h"
#include "map.h"
#include "map_loader.h"
#include "output_buffer.h"
#include "text_output.h"
#include "action.h"
#include "main.h"

/*--------------------------------------------------------------------------*/
int main (int argc, char *argv[]) {
	Map* map = NULL;
	int num_tiles = OUTPUT_IN_GLYPHS_X * OUTPUT_IN_GLYPHS_Y, i;
	BufferTile* buf = NULL;
	Spawn* player = NULL;
	
	if(SDL_Init(SDL_INIT_VIDEO)) {
		return EXIT_FAILURE;
	}
	
	if(argc == 2) {
		map = load_map(argv[1]);
		if(map == NULL) {
			fprintf(stderr, "Karte nicht ladbar!\n");
			return EXIT_FAILURE;
		}
	}
	else{
		fprintf(stderr, "Kartennamen angeben!\n");
		return EXIT_FAILURE;
	}
		
	/* erste Nachricht: map xy geladen */
	message(map, "geladen!");
	
	/* Ausgabepuffer initialisieren */
	buf = (BufferTile*)ex_malloc(sizeof(BufferTile) * num_tiles);
	for(i = 0; i < num_tiles; ++i) {
		BufferTile bt = {' ', 0x00000000};
		buf[i] = bt;
	}
	
	output_init(OUTPUT_IN_GLYPHS_X, OUTPUT_IN_GLYPHS_Y);
	player = get_player_spawn(map);
	
	explore_area(player, map);
	create_output_buffer(map, buf, num_tiles);
	output_draw(buf, num_tiles);
	
	/*Eingabeloop*/
	SDL_Event event;
	while(SDL_WaitEvent(&event)) {
		if(event.type == SDL_KEYDOWN) {
			/*bei Escape beenden*/
			if(event.key.keysym.sym == SDLK_ESCAPE) {
				break;
			}
			process_event(&event, map);
			create_output_buffer(map, buf, num_tiles);
			output_draw(buf, num_tiles);
		} else if(event.type == SDL_QUIT) {
			break;
		}
		SDL_Delay(1);
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
	print_message_box(map, buf, size);
}

/*--------------------------------------------------------------------------*/
 void clear_output_buffer(BufferTile* buf, int num) {
	int i = 0;
	for(; i < num; ++i) {
		buf[i].glyph = ' ';
		buf[i].color = 0x00000000;
	}
 }
 
 /*--------------------------------------------------------------------------*/
 void message(Map* map, char* msg) {
	/* durch den Stream loopen */
	if(map->messages == MESSAGE_STREAM_LIMIT) {
		if(map->current_message == MESSAGE_STREAM_LIMIT - 1) {
			map->current_message = 0;
		} else {
			map->current_message++;
		}
		free(map->message_stream[map->current_message]);
		map->message_stream[map->current_message] = (char*)ex_calloc(strlen(msg) + 1, 1);
		strcpy(map->message_stream[map->current_message], msg);
	} else {
		map->message_stream = (char**)ex_realloc(map->message_stream, sizeof(char*) * ++map->messages);
		map->current_message = map->messages - 1;
		map->message_stream[map->current_message] = (char*)ex_calloc(strlen(msg) + 1, 1);
		strcpy(map->message_stream[map->current_message], msg);
	}
 }
 
 /*--------------------------------------------------------------------------*/
 void print_message_box(Map* map, BufferTile* buf, int num) {
	(void)num;
	/* Border malen */
	int x = OUTPUT_IN_GLYPHS_X * (OUTPUT_IN_GLYPHS_Y - 2), i, j, message_length;
	
	for(i = x; i < x + OUTPUT_IN_GLYPHS_X; ++i) {
		buf[i].color = 0xFFFFFF00;
		buf[i].glyph = '-';
	}
	
	x = OUTPUT_IN_GLYPHS_X * (OUTPUT_IN_GLYPHS_Y - 1);
	
	/* Nachrichten malen */
	message_length = strlen(map->message_stream[map->current_message]);
	for(i = x, j = 0; i < x + (int)(OUTPUT_IN_GLYPHS_X / 3 * 2); ++i, ++j) {
		buf[i].color = 0xFFFFFF00;
		
		if(j < message_length) {
			buf[i].glyph = map->message_stream[map->current_message][j];
		}
	}
	
	/* Spielerstatus malen */
	{
		char* stats = "|HP: %d, Hand: %s", *stats_buffer = NULL;
		Spawn* player = get_player_spawn(map);
		int hp_length = player->hp == 0 ? 1 : log10(player->hp) + 1, strlength;
		
		if(player->inventory != NULL && player->inventory[player->current_item] != NULL) {
			Item* current_item = player->inventory[player->current_item];
			strlength = 16 + strlen(current_item->name) + hp_length;
			stats_buffer = (char*)ex_calloc(strlength, 1);
			sprintf(stats_buffer, stats, player->hp, current_item->name);
		} else {
			strlength = 16 + hp_length;
			stats_buffer = (char*)ex_calloc(strlength, 1);
			sprintf(stats_buffer, stats, player->hp, "--");
		}
		
		for(j = 0; j < strlength && i < x + OUTPUT_IN_GLYPHS_X; ++i, ++j) {
			buf[i].color = 0xFFFFFF00;
			buf[i].glyph = stats_buffer[j];
		}
		free(stats_buffer);
	}
 }
