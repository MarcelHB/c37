/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * sdl_output.h
 */
 
 #ifndef _C37_SDL_OUTPUT_H
 #define _C37_SDL_OUTPUT_H
 
 #include "output_buffer.h"
 
 /* initialisiert Ausgabe */
 void output_init(int, int); 
 /* gibt den Ausgabepuffer mittels SDL aus */
 void output_draw(BufferTile*, int);
 /* löscht die Ausgabe */
 void output_clear();
 /* Ausgabedevices schließen */
 void output_close();
 
 #endif
