/**
 * C-Projekt von Gruppe 37
 * Autor: Marcel
 * Datum: 14.01.11
 * 
 * map_loader.c
 */
 
 #include "globals.h"
 
 /*--------------------------------------------------------------------------*/
 /* JSON_Parser benötigt einen Funktionszeiger als Callback, deshalb nicht über Parameter lösbar */
 static Map intermediate_map = {
	.x = 0, 
	.y = 0,
	.name = NULL, 
	.tiles = NULL,
	.spawns = NULL,
	.number_of_spawns = 0
 };
 static unsigned int array_depth = 0;
 static unsigned int has_root = 0;
 static int parsed_tiles = -1;
 static int parsed_spawns = -1;
 static unsigned int* node_stack = NULL;
 static unsigned int node_stack_size = 0;
 /*--------------------------------------------------------------------------*/
 
 Map* load_map(char* name) {
	FILE* in = NULL;
	Map* map = NULL;
	JSON_config config;
	struct JSON_parser_struct* jc = NULL;
	int i;
	char* path_and_name = (char*)ex_calloc(strlen(MAP_DIRECOTRY) + strlen(name) + strlen(MAP_FILE_EXTENSION) + 2, 1);
	
	/* "maps/[name].json" */
	strcpy(path_and_name, MAP_DIRECTORY); strcat(path_and_name, "/"); strcat(path_and_name, name); strcat(path_and_name, MAP_FILE_EXTENSION);
	
	in = fopen(path_and_name, "r");
	if(in == NULL) {
		return NULL;
	}
	
	reset_parse_stats();
	
	init_JSON_config(&config);	
	config.depth                  = 5;
    config.callback               = &parse;
    config.allow_comments         = 1;
    config.handle_floats_manually = 0;
	
	jc = new_JSON_parser(&config);
	
	for (i = 0; map_file; ++i) {
        int next_char = fgetc(map_file);
        if (next_char == EOF) {
            break;
        }
		/* Syntaxfehler, unerwartetes Zeichen */
        if (!JSON_parser_char(jc, next_char)) {
			fprintf(stderr, "JSON_parser_char: syntax error, byte %d\n", i);
			clean_up_parsing(jc, path_and_name, map_file);
            return NULL;
        }
    }
	
	/* JSON z.B. nicht abgeschlossen */
	if (!JSON_parser_done(jc)) {
		fprintf(stderr, "JSON_parser_end: syntax error\n");
		clean_up_parsing(jc, path_and_name, map_file);
		return NULL;
    }
	
	/* kopiert die Zwischenkarte in diesem Scope in einen eigenen Speicherbereich */
	map = finalize_map(map);
	
	clean_up_parsing(jc, path_and_name, map_file);
	return map;	
 }
 
 /*--------------------------------------------------------------------------*/
 void flush_map(Map* map) {
	int i;
	/* Name */
	free(map->name);
	
	/* Kacheln */
	for(i = 0; i < map->x * map->y; ++i) {
		free_tile(&(map->tiles[i]));
	}
	free(map->tiles);
	
	/* Spawns */
	for(i = 0; i < map->number_of_spawns; ++i) {
		free_spawn(&(map->spawns[i]));
	}
	free(map->spawns);
	
	free(map);
 }
 
 /*--------------------------------------------------------------------------*/
 void clean_up_parsing(JSON_parser_struct* parser, char* name, FILE* map_file) {
	delete_JSON_parser(parser);
	free(name);
	fclose(map_file);
 }
 
 /*--------------------------------------------------------------------------*/
 /* Callback-Funktion für JSON_parser */
 static int parse(void* cfg, int type, const JSON_value* value) {
 
	switch(type) {
		/* { */
		case JSON_T_OBJECT_BEGIN:
			parse_object_begin(value, &intermediate_map, &node_stack, &node_stack_size, &has_root, &parsed_tiles, parsed_spawns, array_depth);
			break;
		/* } */
		case JSON_T_OBJECT_END:
			parse_object_end(&node_stack, &node_stack_size, array_depth);
			break;
		/* [ */
		case JSON_T_ARRAY_BEGIN:
			parse_array_begin(node_stack, node_stack_size, &array_depth);
			break;
		/* ] */
		case JSON_T_ARRAY_END:
			parse_array_end(&node_stack, &node_stack_size, &array_depth);
			break;
		/* TODO! */
	}
	
	return 1;
 }
 
 /*--------------------------------------------------------------------------*/
 /* Parser-Wert, Karte, Tiefenstack, Größe davon, Root-Marker, Anz. geparster Kacheln, Anz. geparster Spawns, Arraytiefe */
 void parse_object_begin(const JSON_value* value, Map* map, unsigned int** stack, unsigned int* stack_size, unsigned int* has_root, int* parsed_tiles, int* parsed_spawns, unsigned int array_depth) {
	if(!(*has_root)) {
		push_node_stack(NODE_ROOT, &stack, stack_size);
		*has_root = 1;
	} else {
		int stack_top = node_stack_at(0, *stack, stack_size);
		int parent = node_stack_at(1, *stack, stack_size);
		
		/* Root->Kacheln */
		if(stack_top != STACK_INVALID_INDEX && parent == STACK_ROOT && stack_top == STACK_ROOT_TILES) {
			if(array_depth == 2 && (map->x * map->y) > 0) {
				(*parsed_tiles)++;
			}
		}
		/* Root->Spawns */
		if(stack_top != STACK_INVALID_INDEX && parent == STACK_ROOT && stack_top == STACK_ROOT_SPAWNS) {
			if(array_depth == 2) {
				(*parsed_spawns)++;
			}
		}
	}
 }
 
 /*--------------------------------------------------------------------------*/
 void parse_object_end(unsigned int** stack, unsigned int* stack_size, unsigned int array_depth) {
	int stack_top = node_stack_at(0, *stack, stack_size);
	
	if(stack_top != STACK_INVALID_INDEX && parse_array_depth == 0) {
		pop_node_stack(stack, stack_size);
	}
 }
 
 /*--------------------------------------------------------------------------*/
 void parse_array_begin(int* stack, unsigned int stack_size, unsigned int* array_depth) {
	int stack_top = node_stack_at(0, stack, stack_size);
	
	if(stack_top != STACK_INVALID_INDEX && (stack_top == STACK_ROOT_TILES || stack_top == STACK_ROOT_SPAWNS)) {
		(*parse_array_depth)++;
	}
 }
 
 /*--------------------------------------------------------------------------*/
 void parse_array_end(unsigned int** stack, unsigned int* stack_size, unsigned int* array_depth) {
	int stack_top = node_stack_at(0, *stack, stack_size);
 
	if(stack_top != STACK_INVALID_INDEX && (stack_top == STACK_ROOT_TILES || stack_top == STACK_ROOT_SPAWNS)) {
		(*array_depth)--;
		if((*array_depth) == 0) {
			pop_node_stack(stack, stack_size)
		}
	}
 }
 
 /*--------------------------------------------------------------------------*/
 unsigned int node_stack_at(unsigned int index, int* stack, unsigned int stack_size) {
	/* Stack leer oder index außerhalb der Größe */
	if(!stack_size || ((stack_size - index) <= 0)) {
		return STACK_INVALID_INDEX;
	}
	
	return stack[stack_size - index - 1];
 }
 
 /*--------------------------------------------------------------------------*/
 void push_node_stack(int node, unsigned int** stack, unsigned int* stack_size) {
	/* Counter hoch */
	int new_stack_size = ++(*stack_size);
	/* vergrößern */
	stack = (int*)ex_realloc(stack, sizeof(int) * new_stack_size);
	/* kopieren */
	stack[new_stack_size - 1] = node;
 }
 
 /*--------------------------------------------------------------------------*/
 unsigned int pop_node_stack(unsigned int** stack, unsigned int* stack_size) {
	int tmp;
	int elements = *stack_size;
	if(!stack_size) {
		return STACK_INVALID_INDEX;
	}
	
	/* temp. Speichern des Top-Elements */
	tmp = stack[elements - 1];
	/* POP */
	elements = --(*stack_size);
	
	/* verkleinern */
	stack = (int*)ex_realloc(stack, sizeof(int) * elements);
	return tmp;
 }
 
 /*--------------------------------------------------------------------------*/
 Map* finalize_map(Map* map) {
	size_t map_size = sizeof(Map);
	map = (Map*)ex_alloc(map_size);
	/* bewahre Pointer */
	memcpy(map, &intermediate_map, map_size);
	reset_intermediate_map();
	return map;
 }
 
 /*--------------------------------------------------------------------------*/
 /* nur benutzen, wenn der reservierte Speicher für Name, Tiles usw. irgendwo 
	anders noch referenziert wird! */
 void reset_intermediate_map() {
	intermediate_map.x = 0;
	intermediate_map.y = 0;
	intermediate_map.name = NULL;
	intermediate_map.tiles = NULL;
	intermediate_map.spawns = NULL;
	intermediate_map.number_of_spawns = 0;
 }
 
 /*--------------------------------------------------------------------------*/
 void reset_parse_stats() {
	array_depth = 0;
    has_root = 0;
    parsed_tiles = -1;
    parsed_spawns = -1;
    node_stack = NULL;
    node_stack_size = 0;
 }