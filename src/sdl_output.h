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
 /* l�scht die Ausgabe */
 void clear();
 /* Ausgabedevices schlie�en */
 void close();
 
 #endif