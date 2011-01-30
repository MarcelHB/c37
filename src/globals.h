/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * globals.h
 */
 
 #ifndef _C37_GLOBALS_H
 #define _C37_GLOBALS_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <math.h>
 
 /* interne Settings */
 #define OUTPUT_IN_GLYPHS_X 80
 #define OUTPUT_IN_GLYPHS_Y 25
 #define MESSAGE_STREAM_LIMIT 100
 #define MIN_VISIBILTY_BRIGHTNESS 30
 
 /* einige Spielregeln */
 #define VISUAL_SQUARE 5
 
 /* Himmelsrichtungen: 0000NESW */
 #define NORTH 8
 #define EAST 4
 #define SOUTH 2
 #define WEST 1
 
 #define MAP_DIRECTORY "maps"
 #define MAP_FILE_EXTENSION ".json"
 
 /* #define _C37_MEMORY_LOG */
 
 #endif