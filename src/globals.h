﻿/**
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
 
 /* Wenn nicht für Ausgabe, dann für Tastatur */
 #include "SDL.h"
 
 #include "memory.h"
 #include "game_messages.h"
 #include "output_buffer.h"
 #include "item.h"
 #include "spawn.h"
 #include "tile.h"
 #include "map.h"
 #include "map_loader.h"
 #include "main.h"
 
 /* interne Settings */
 #define OUTPUT_IN_GLYPHS_X 80
 #define OUTPUT_IN_GLYPHS_Y 25
 #define MESSAGE_STREAM_LIMIT 100
 
 /* einige Spielregeln */
 #define VISUAL_SQUARE 5
 
 /* Himmelsrichtungen: 0000NESW */
 #define NORTH 8
 #define EAST 4
 #define SOUTH 2
 #define WEST 1
 
 #define MAP_DIRECOTRY "maps"
 #define MAP_FILE_EXTENSION ".json"
 
 /* bis jetzt nur SDL */
 #include "sdl_output.h"
 
 #endif