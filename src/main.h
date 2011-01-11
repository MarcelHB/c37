/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * main.h
 */

 #ifndef _C37_MAIN_H
 #define _C37_MAIN_H
 
 /* Kopiert den sichtbaren Bereich mit kalk. Zeichen- & Farbinfos für Ausgabe */
 void create_output_buffer(Map*, OutputBuffer*, int);
 /* Male die Nachrichtenausgabe darüber */
 void print_message_box(OutputBuffer*, int, char**, int);
 /* Übermalt Ausgabepuffer mit nil-Werten */
 void flush_output_buffer(OutputBuffer*, int);
 
 #endif