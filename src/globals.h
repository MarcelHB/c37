/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * globals.h
 */
 
 #ifndef _C37_GLOBALS_H
 #define _C37_GLOBALS_H
 
 #include <stdlib.h>
 
 /* Wenn nicht für Ausgabe, dann für Tastatur */
 #include "SDL.h"
 
 #include "memory.h"
 #include "output_buffer.h"
 #include "item.h"
 #include "spawn.h"
 #include "tile.h"
 #include "map.h"
 #include "map_loader.h"
 #include "main.h"
 
 /* interne Settings */
 #define OUTPUT_SDL 1
 #define OUTPUT_IN_GLYPHS_X 80
 #define OUTPUT_IN_GLYPHS_Y 25
 #define MESSAGE_STREAM_LIMIT 100
 
 /* einige Spielregeln */
 #define VISUAL_SQUARE 5
 
 /* bis jetzt nur SDL */
 #ifdef OUTPUT_SDL
 #include "sdl_output.h"
 #end
 
 #endif