/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * text_output.h
 */
 
 #ifndef _C37_TEXT_OUTPUT_H
 #define _C37_TEXT_OUTPUT_H
 
 /* initialisiert Ausgabe */
 void output_init(int, int); 
 /* gibt den Ausgabepuffer als einfachen Text auf der Konsole aus
  * (Farben werden ignoriert) */
 void output_draw(BufferTile*, int);
 /* löscht die Ausgabe */
 void output_clear();
 /* Ausgabedevices schließen */
 void output_close();
 
 #endif
