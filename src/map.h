/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * map.h
 */
 
 #ifndef _C37_MAP_H
 #define _C37_MAP_H

#include <stdbool.h>
 
 #include "item.h"
 #include "tile.h"
 #include "spawn.h"
 #include "map.h"
 
 /* Struktur einer Karte */
 typedef struct Map {
	/* Dimension */
	unsigned int x, y;
	/* Name */
	char* name;
	/* Bestandteile */
	Tile* tiles;
	/* Actors */
	Spawn** spawns;
	unsigned int number_of_spawns;
 } Map;

bool map_can_see (Map *, int, int, int, int);
 
 /* Findet den Spieler */
 Spawn* get_player_spawn(Map*);
 /* Spawn an Position oder NULL */
 Spawn* get_spawn_at(unsigned int, unsigned int, Map*); 
 
 /* war mal spawn.h */
 /* Wenn ein Spawn auf einein anderen trifft, gucke, was hier passiert (schubsen/töten/sprechen/...) */
 void spawn_spawn_collision(Spawn*, Spawn*, Map*);
 /* Deckt die unerkundete Gegend um einen Spawn herum auf. */
 void explore_area(Spawn*, Map*);
 /* Der Spawn ist am Zug, etwas zu tun. */
 void spawn_action(Spawn*, Map*);
 /* Spwan führt die Action eines Items aus (Trank trinken...) */
 void spawn_uses_item(Spawn*, Item*, Map*);
 
 /* war mal tile.h */
 /* Wenn ein Spawn auf ein Tile trifft, gucke, was hier passiert (rennt gegen Wand/stirbt/sammelt auf/...) */
 void spawn_tile_collision(Spawn*, Tile*, Map*, char**, int);
 /* irgendwas wird getoggelt (Tür auf, zu ...) */
 void toggle_tile(Tile*, Map*);
 
 #endif
