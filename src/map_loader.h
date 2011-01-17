/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * map_loader.h
 */
 
 #ifndef _C37_MAP_LOADER_H
 #define _C37_MAP_LOADER_H
 
 #include "JSON_parser.h"
 
 /* lädt die Karte anhand ihres Dateinamens (ohne Endung) und gibt einen Status (success) zurück */
 Map* load_map(char*);
 /* entfernt die Karte aus dem Speicher */
 void flush_map(Map*);
 
 /* internals, Helper */
 void clean_up_parsing(JSON_parser_struct*, char*, FILE*);
 Map* finalize_map(Map*);
 
 void reset_intermediate_map();
 void reset_parse_stats();
 
 static int parse(void*, int, const JSON_value*);
 void parse_object_begin(Map*, unsigned int**, unsigned int*, unsigned int*, int*, unsigned int);
 void parse_object_end(unsigned int**, unsigned int*, unsigned int);
 void parse_array_begin(unsigned int*, unsigned int, unsigned int*);
 void parse_array_end(unsigned int**, unsigned int*, unsigned int*);
 void parse_key(const JSON_value*, Map*, unsigned int**, unsigned int*, const unsigned int, const int);
 void parse_string(const JSON_value*, Map*, unsigned int**, unsigned int*, const int, const unsigned int);
 void parse_integer(const JSON_value*, Map*, unsigned int**, unsigned int*, const int, const unsigned int);
 
 unsigned int node_stack_at(unsigned int, unsigned int*, unsigned int);
 void push_node_stack(int, unsigned int**, unsigned int*);
 unsigned int pop_node_stack(int*, unsigned int*);
 void calculate_tile_id(Tile*, const int, const int);
 void calculate_spawn_id(Spawn*, const int);
 
 int tile_property_identifier(const char*);
 int spawn_property_identifier(const char*);
 unsigned int tile_type_id(const char*);

 /* Object-Keys beim Parsen */
 
 /* Root (Map)-Keys */
 #define NODE_ROOT 						"root"
 #define NODE_NAME 						"name"
 #define NODE_X 						"x"
 #define NODE_Y 						"y"
 #define NODE_TILES 					"tiles"
 #define NODE_SPAWNS 					"spawns"
 #define NODE_TYPE	 					"type"
 #define NODE_ID	 					"id"
 #define NODE_ITEMS	 					"items"
 #define NODE_BRIGHTNESS				"brightness"
 #define NODE_DIRECTION					"direction"
 #define NODE_HEALTHPOINTS				"hp"
 #define NODE_MAX_HEALTHPOINTS			"max_hp"
 #define NODE_ITEMS						"items"
 
 /* Stack-Identifier */
 #define STACK_INVALID_INDEX			0xFFFFFFFF
 #define STACK_ROOT 					0x00000000
 #define STACK_NAME 					0x00000001
 #define STACK_X 						0x00000002
 #define STACK_Y 						0x00000003
 #define STACK_TILES 					0x00000004
 #define STACK_SPAWNS 					0x00000005
 #define STACK_TYPE 					0x00000006
 #define STACK_ID						0x00000007
 #define STACK_ITEMS					0x00000008
 #define STACK_BRIGHTNESS				0x00000009
 #define STACK_DIRECTION				0x0000000A
 #define STACK_HEALTHPOINTS				0x0000000B
 #define STACK_MAX_HEALTHPOINTS			0x0000000C
 #define STACK_ITEMS					0x0000000D

 /* lesbare Tile-Type Namen */
 #define TILE_NAME_WALL					0x00000000
 #define TILE_NAME_FLOOR				0x00000001
 #define TILE_NAME_BUTTON				0x00000002
 #define TILE_NAME_DOOR					0x00000003
 
 #endif