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
 void parse_object_begin(const JSON_value*, Map*, unsigned int**, unsigned int*, unsigned int*, int*, int*, unsigned int);
 void parse_object_end(unsigned int**, unsigned int*, unsigned int);
 void parse_array_begin(unsigned int*, unsigned int, unsigned int*);
 void parse_array_end(unsigned int**, unsigned int*, unsigned int*);
 
 unsigned int node_stack_at(unsigned int, unsigned int*, unsigned int);
 void push_node_stack(int, unsigned int**, unsigned int*);
 unsigned int pop_node_stack(int*, unsigned int*);

 /* Object-Keys beim Parsen */
 
 /* Root (Map)-Keys */
 #define NODE_ROOT "root"
 #define NODE_ROOT_NAME "name"
 #define NODE_ROOT_X "x"
 #define NODE_ROOT_Y "y"
 #define NODE_ROOT_TILES "tiles"
 #define NODE_ROOT_SPAWNS "spawns"
 
 /* Stack-Identifier */
 #define STACK_INVALID_INDEX			0xFFFFFFFF
 #define STACK_ROOT 					0x00000000
 #define STACK_ROOT_NAME 				0x00000001
 #define STACK_ROOT_X 					0x00000002
 #define STACK_ROOT_Y 					0x00000003
 #define STACK_ROOT_TILES 				0x00000004
 #define STACK_ROOT_SPAWNS 				0x00000005
 
 #endif