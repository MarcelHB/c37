/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * tiles/button.h
 */

 #ifndef _C37_TILE_BUTTON_H
 #define _C37_TILE_BUTTON_H
 
 /* Wand */
 typedef struct ButtonProperties {
	/* ID des toggle (ein Tile) */;
	char* toggle_id;
	/* nur einmal drückbar? */
	char once;
	/* aktiviert? */
	char active;
	/* von wo aus bedienbar? 0000NESW */;
	char directions;
 } ButtonProperties;
 
 #endif