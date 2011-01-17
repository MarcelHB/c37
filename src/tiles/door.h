/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * tiles/door.h
 */

 #ifndef _C37_TILE_DOOR_H
 #define _C37_TILE_DOOR_H
 
 /* Tür */
 typedef struct DoorProperties {
	/* horizontal? (0 = vertikal) */
	char horizontal;
	/* Zustand */
	char open;
	/* wird woanders aufgemacht? */
	char external_button;
	/* eintretbar? */
	char breakable;
	char broken;
 } DoorProperties;
 
 #endif