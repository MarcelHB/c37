/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * main.h
 */

 #ifndef _C37_MAIN_H
 #define _C37_MAIN_H
 
 #include "output_buffer.h"
 #include "map.h"
 #include "map_loader.h"
 
 /* Kopiert den sichtbaren Bereich mit kalk. Zeichen- & Farbinfos für Ausgabe */
 void create_output_buffer(Map*, BufferTile*, int);
 /* Male die Nachrichtenausgabe darüber */
 void print_message_box(Map*, BufferTile*, int);
 /* Übermalt Ausgabepuffer mit nil-Werten */
 void clear_output_buffer(BufferTile*, int);
  /* Nachricht hinzufuegen */
 void message(Map*, char*);
 
 #endif