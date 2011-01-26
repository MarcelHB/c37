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
	/* Message-History */
	char** msg_hist;
	int latest_msg;
	int current_msg;
	/* Map beendet? */
	char finished;
 } Map;

bool map_can_see (Map *, unsigned int, unsigned int, unsigned int, unsigned int);
 
 /* Findet den Spieler */
 Spawn* get_player_spawn(Map*);
 /* Spawn an Position oder NULL */
 Spawn* get_spawn_at(unsigned int, unsigned int, Map*); 
 /* Deckt die unerkundete Gegend um einen Spawn herum auf. */
 void explore_area(Spawn*, Map*);
 /* schreibt die tatsächliche Anzeige (Spawns/Items drauf?) auf diesem Tile auf den BufferTile */
 void render_tile(BufferTile*, Tile*, Map*);
 /*neue Nachricht auf die History packen*/
 void push_msg(char *msg, Map *map);

#endif
