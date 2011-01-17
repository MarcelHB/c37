/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * map.h
 */
 
 #ifndef _C37_MAP_H
 #define _C37_MAP_H
 
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
 
 /* Findet den Spieler */
 Spawn* get_player_spawn(Map*);
 /* Spawn an Position oder NULL */
 Spawn* get_spawn_at(unsigned int, unsigned int, Map*); 
  
 /* sämtliche create-Funktionen */
 /* ... */
 
 #endif