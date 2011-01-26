/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * tile.h
 */

 #ifndef _C37_TILE_H
 #define _C37_TILE_H

#include <stdbool.h>
 
 #include "output_buffer.h"
 #include "item.h"

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
	Item** items;
	unsigned int number_of_items;
 } Tile;
 
 /* Mapbestandteile */
 #include "tiles/wall.h"
 #include "tiles/button.h"
 #include "tiles/door.h"
 #include "tiles/water.h"
 #include "tiles/hint.h"
 
 /* verwendete Tile-type identifier */
 #define TILE_TYPE_INVALID				0xFFFFFFFF
 #define TILE_TYPE_WALL					0x00000000
 #define TILE_TYPE_FLOOR				0x00000001
 #define TILE_TYPE_BUTTON				0x00000002
 #define TILE_TYPE_DOOR					0x00000003
 #define TILE_TYPE_WATER				0x00000004
 #define TILE_TYPE_HINT					0x00000005
 
 #define TILE_GLYPH_WALL				'#'
 #define TILE_GLYPH_FLOOR				'.'
 #define TILE_GLYPH_BUTTON				'+'
 #define TILE_GLYPH_BUTTON				'+'
 #define TILE_GLYPH_DOOR_VERT_CLOSED	'|'
 #define TILE_GLYPH_DOOR_VERT_OPEN		'~'
 #define TILE_GLYPH_DOOR_HORIZ_CLOSED 	'-'
 #define TILE_GLYPH_DOOR_HORIZ_OPEN 	'/'
 #define TILE_GLYPH_WATER				'~'
 #define TILE_GLYPH_HINT				'?'
 
 /* Zuordnungstabellen von type -> defaults */
 struct TileDefault {
	unsigned int type;
	unsigned long color;
	char glyph;
 };
 
 /* lediglich Intialisierungswerte */
 static const struct TileDefault tile_defaults[6] = {
	{TILE_TYPE_WALL, 0xAAAAAA00, TILE_GLYPH_WALL},
	{TILE_TYPE_FLOOR, 0xFFFFFF00, TILE_GLYPH_FLOOR},
	{TILE_TYPE_BUTTON, 0xFFFFFF00, TILE_GLYPH_BUTTON},
	{TILE_TYPE_DOOR, 0xCCCCCC00, TILE_GLYPH_DOOR_VERT_CLOSED},
	{TILE_TYPE_WATER, 0x00006600, TILE_GLYPH_WATER},
	{TILE_TYPE_HINT, 0xFFFFFF00, TILE_GLYPH_HINT}
 };
 
 /* Gibt reservierten Speicher eines Tiles frei */
 void free_tile(Tile*);
 
 /* Helper */
 void apply_tile_defaults(Tile*);
 void create_tile_properties(Tile*);

 bool tile_can_light(Tile*);
 bool tile_can_walk(Tile*);
 
 #endif
