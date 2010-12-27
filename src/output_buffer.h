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
 
 #endif