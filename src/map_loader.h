/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * map_loader.h
 */
 
 #ifndef _C37_MAP_LOADER_H
 #define _C37_MAP_LOADER_H
 
 #include "globals.h"
 #include "item.h"
 #include "spawn.h"
 #include "tile.h"
 #include "map.h"
 #include "JSON_parser.h"
 
 /* lädt die Karte anhand ihres Dateinamens (ohne Endung) und gibt einen Status (success) zurück */
 Map* load_map(char*);
 /* entfernt die Karte aus dem Speicher */
 void flush_map(Map*);
 
 /* internals, Helper */
 void clean_up_parsing(JSON_parser, char*, FILE*, unsigned int*);
 Map* finalize_map(Map*);
 
 void reset_intermediate_map();
 void reset_parse_stats();
 
 void parse_object_begin(Map*, unsigned int**, unsigned int*, unsigned int*, unsigned int*, unsigned int);
 void parse_object_end(unsigned int**, unsigned int*, unsigned int);
 void parse_array_begin(Map*, unsigned int*, unsigned int, unsigned int*);
 void parse_array_end(unsigned int**, unsigned int*, unsigned int*);
 void parse_key(const JSON_value*, unsigned int**, unsigned int*, const unsigned int);
 void parse_string(const JSON_value*, Map*, unsigned int**, unsigned int*, const unsigned int, const unsigned int);
 void parse_integer(const JSON_value*, Map*, unsigned int**, unsigned int*, const unsigned int, const unsigned int, char*);
 void parse_bool(const unsigned int, Map*, unsigned int**, unsigned int*, const unsigned int, const unsigned int);
 
 void parse_string_property(const JSON_value*, Map*, const unsigned int, const unsigned int, const unsigned int, const unsigned int);
 void parse_integer_property(const JSON_value*, Map*, const unsigned int, const unsigned int, const unsigned int, const unsigned int);
 void parse_bool_property(const unsigned int, Map*, const unsigned int, const unsigned int, const unsigned int, const unsigned int);
 
 unsigned int node_stack_at(unsigned int, unsigned int*, unsigned int);
 void push_node_stack(unsigned int, unsigned int**, unsigned int*);
 unsigned int pop_node_stack(unsigned int**, unsigned int*);
 void calculate_tile_id(Tile*, const unsigned int, const unsigned int);
 void calculate_spawn_id(Spawn*, const unsigned int);
 void calculate_item_id(Item*, const char*, const unsigned int);
 
 unsigned int tile_property_identifier(const char*);
 unsigned int item_property_identifier(const char*);
 unsigned int tile_type_id(const char*);
 unsigned int spawn_type_id(const char*);
 unsigned int item_type_id(const char*);

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
 
 #define NODE_DIRECTION					"dir"
 #define NODE_HEALTHPOINTS				"hp"
 #define NODE_MAX_HEALTHPOINTS			"max_hp"
 #define NODE_ITEMS						"items"
 
 #define NODE_SPACE						"space"
 #define NODE_ONCE						"once"
 #define NODE_TOGGLE_ID					"tid"
 #define NODE_DIRECTIONS				"dirs"
 #define NODE_ACTIVE					"active"
 #define NODE_HORIZONTAL				"horz"
 #define NODE_OPEN						"open"
 #define NODE_EXTERNAL					"ext"
 #define NODE_LOCKED					"locked"
 #define NODE_BREAKABLE					"breaks"
 #define NODE_DEPTH						"depth"
 
 #define NODE_VALUE						"val"
 #define NODE_WEIGHT					"weight"
 
 #define NODE_CAPACITY					"cap"
 
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
 #define STACK_VALUE					0x0000000D
 #define STACK_SPACE					0x0000000E
 #define STACK_ONCE						0x0000000F
 #define STACK_TOGGLE_ID				0x00000010
 #define STACK_DIRECTIONS				0x00000011
 #define STACK_ACTIVE					0x00000012
 #define STACK_HORIZONTAL				0x00000013
 #define STACK_OPEN						0x00000014
 #define STACK_EXTERNAL					0x00000015
 #define STACK_LOCKED					0x00000016
 #define STACK_BREAKABLE				0x00000017
 #define STACK_WEIGHT					0x00000018
 #define STACK_CAPACITY					0x00000019
 #define STACK_DEPTH					0x00000020

 /* lesbare Tile-Type Namen */
 #define TILE_NAME_WALL					"w"
 #define TILE_NAME_FLOOR				"f"
 #define TILE_NAME_BUTTON				"btn"
 #define TILE_NAME_DOOR					"door"
 #define TILE_NAME_WATER				"h2o"
 
 /* lesbare Spawn-Type Namen */
 #define SPAWN_NAME_PLAYER				"player"
 
 /* lesbare Item-Type Namen */
 #define ITEM_NAME_HEALTH_POTION		"hptn"
 
 #endif