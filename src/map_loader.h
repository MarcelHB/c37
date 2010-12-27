/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * map_loader.h
 */
 
 #ifndef _C37_MAP_LOADER_H
 #define _C37_MAP_LOADER_H
 
 /* Lädt die Karte anhand ihres Dateinamens (ohne Endung) und gibt einen Status (success) zurück */
 int load_map(const char*);
 /* Entfernt die Karte aus dem Speicher */
 void flush_map();

 /* sämtliche create-Funktionen */
 /* ... */
 
 #endif