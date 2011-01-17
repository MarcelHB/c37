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
	unsigned int type;
	/* spez. Eigenschaften des Kacheltyps */
	void* properties;
	/* Items, die hier liegen */
	Item* items;
	unsigned int number_of_items;
 } Tile;
 
 /* Mapbestandteile */
 #include "tiles/wall.h"
 #include "tiles/button.h"
 #include "tiles/door.h"
 
  /* verwendete Tile-type identifier */
 #define TILE_TYPE_INVALID				0xFFFFFFFF
 #define TILE_TYPE_WALL					0x00000000
 #define TILE_TYPE_FLOOR				0x00000001
 #define TILE_TYPE_BUTTON				0x00000002
 #define TILE_TYPE_DOOR					0x00000003
 
 /* Zuordnungstabellen von type -> defaults */
 struct TileDefault {
	unsigned int type;
	unsigned long color;
	char glyph;
 };
 
 /* lediglich Intialisierungswerte */
 const struct TileDefault tile_defaults[4] = {
	{TILE_TYPE_WALL, 0xFFFFFF00, '#'},
	{TILE_TYPE_FLOOR, 0xFFFFFF00, '.'},
	{TILE_TYPE_BUTTON, 0xFFFFFF00, '+'},
	{TILE_TYPE_DOOR, 0xCCCCCC00, '|'}
 };
 
 /* schreibt die tatsächliche Anzeige (Spawns/Items drauf?) auf diesem Tile auf den BufferTile */
 void render_tile(BufferTile*, Tile*);
 /* Wenn ein Spawn auf ein Tile trifft, gucke, was hier passiert (rennt gegen Wand/stirbt/sammelt auf/...) */
 void spawn_tile_collision(Spawn*, Tile*, Map*, char**, int);
 /* irgendwas wird getoggelt (Tür auf, zu ...) */
 void toggle_tile(Tile*, Map*);
  /* Gibt reservierten Speicher eines Tiles frei */
 void free_tile(Tile*);
 
 /* Helper */
 void apply_tile_defaults(Tile* tile);
 void create_tile_properties(Tile* tile);
 
 #endif