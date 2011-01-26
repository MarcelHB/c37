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
 void output_init(int, int); 
 /* gibt den Ausgabepuffer mittels SDL aus */
 void output_draw(BufferTile*, int);
 /* löscht die Ausgabe */
 void output_clear();
 /* Ausgabedevices schließen */
 void output_close();
 /* HP in der Statusleiste aktualisieren */
 void update_hp(int);
 /* Item in der Statusleiste aktualisieren */
 void update_item(char*);
 /* Mitteilung schreiben */
 void print_msg(char*);
 /* GAME OVER */
 void game_over();
 
 #endif
