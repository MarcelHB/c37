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
 void output_init(); 
 /* gibt den Ausgabepuffer mittels SDL aus */
 void output_draw();
 /* löscht die Ausgabe */
 void output_clear();
 /* Ausgabedevices schließen */
 void output_close();
 
 #endif