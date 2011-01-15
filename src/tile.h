/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * tile.h
 */

 #ifndef _C37_TILE_H
 #define _C37_TILE_H

 typedef struct Tile {
	/* ID */
	char* id;
    /* Platzierung */
	unsigned int x,y;
	/* erforscht? */
	char spotted;
	/* Helligkeit? */
	unsigned int brightness;
	/* ASCII-Zeichen zur Anzeige */
	char glyph;
	/* 32-bit Farbe nach RGBA */
	unsigned long color;
	/* Polymorphismusersatz für den eig. Typ */
	char* type;
	/* spez. Eigenschaften des Kacheltyps */
	void* properties;
	/* Items, die hier liegen */
	Item* items;
	unsigned int number_of_items;
 } Tile;
 
 /* Mapbestandteile */
 #include "tiles/wall.h"
 #include "tiles/button.h"
 
  /* Zuordnungstabellen von type -> defaults */
 /* "wall" -> "255", grau, "#" ... */
 
 /* schreibt die tatsächliche Anzeige (Spawns/Items drauf?) auf diesem Tile auf den BufferTile */
 void render_tile(BufferTile*, Tile*);
 /* Wenn ein Spawn auf ein Tile trifft, gucke, was hier passiert (rennt gegen Wand/stirbt/sammelt auf/...) */
 void spawn_tile_collision(Spawn*, Tile*, Map*, char**, int);
 /* irgendwas wird getoggelt (Tür auf, zu ...) */
 void toggle_tile(Tile*, Map*);
 
 /* Gibt reservierten Speicher eines Tiles frei */
 void free_tile(Tile*);
 
 #endif