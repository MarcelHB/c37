/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * main.h
 */

 #ifndef _C37_MAIN_H
 #define _C37_MAIN_H
 
 /* Hält die Ausführung an */
 void pause_game();
 /* Setzt die Ausführung fort */
 void continue_game(); 
 /* Kopiert den sichtbaren Bereich mit kalk. Zeichen- & Farbinfos für Ausgabe */
 void create_output_buffer();
 /* Male die Nachrichtenausgabe darüber */
 void print_message_box();
 /* Übermalt Ausgabepuffer mit nil-Werten */
 void flush_output_buffer();
 
 #endif