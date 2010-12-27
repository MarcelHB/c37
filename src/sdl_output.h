/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * sdl_output.h
 */
 
 #ifndef _C37_SDL_OUTPUT_H
 #define _C37_SDL_OUTPUT_H
 
 /* initialisiert Ausgabe */
 void init(); 
 /* gibt den Ausgabepuffer mittels SDL aus */
 void draw();
 /* löscht die Ausgabe */
 void clear();
 /* Ausgabedevices schließen */
 void close();
 
 #endif