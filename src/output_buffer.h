/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * output_buffer.h
 */

 #ifndef _C37_OUTPUT_BUFFER_H
 #define _C37_OUTPUT_BUFFER_H
 
 typedef struct BufferTile {
	/* ASCII-Zeichen */
	char glyph;
	/* 32-bit Farbe nach RGBA */
	unsigned long color;
 } BufferTile;
 
typedef struct InterfaceData {
	/* Spieler HP */
	unsigned int player_hp;
	/* ausgewähltes Item */
	char* item_name;
	/* Indexnummer dessen */
	int item_index;
	/* anzuzeigende Nachricht */
	char* message;
	/* aktuelleste? */
	char last_message;
 } InterfaceData;
 
 #endif