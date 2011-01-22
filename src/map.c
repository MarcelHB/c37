/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * map.c
 */

#include "globals.h"
#include "spawn.h"
#include "map.h"

/*--------------------------------------------------------------------------*/
Spawn* get_player_spawn(Map* map) {
	unsigned int i;
	Spawn* spawn = NULL;
	
	for(i = 0; i < map->number_of_spawns; ++i) {
		spawn = map->spawns[i];
		if(!spawn->npc) {
			return spawn;
		}
	}
	
	return NULL;
}

 /*--------------------------------------------------------------------------*/
 void render_tile(BufferTile* buf, Tile* tile, Map* map) {
	unsigned int i;
	Spawn* spawn = NULL;
	
	for(i = 0; i < map->number_of_spawns; ++i) {
		Spawn* candidate = map->spawns[i];
		if(candidate->x == tile->x && candidate->y == tile->y) {
			spawn = candidate;
			break;
		}
	}
	/* Spawn mit Glyphe und Zustandsfarbe rendern */
	if(spawn != NULL) {
		float hp_percentage = (float)spawn->hp / spawn->max_hp;
		unsigned long color = 0;
		/* Zustand: Rotton */
		color = (1 - hp_percentage) * 0xFF;
		/* Zustand: Grünton */
		color = (color << 8) | (unsigned int)((hp_percentage) * 0xFF);
		color <<= 16;
		buf->glyph = spawn->glyph;
		buf->color = color;
	} else{
		/* Item */
		if(tile->number_of_items > 0) {
			buf->glyph = '*';
			buf->color = tile->items[0]->color;
		} else {
			apply_tile_defaults(tile);
			buf->glyph = tile->glyph;
			buf->color = tile->color;
		}
	}
 }