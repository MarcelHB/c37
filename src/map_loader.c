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
	map->tiles = NULL;
	
	/* Spawns */
	for(i = 0; i < map->number_of_spawns; ++i) {
		free_spawn(&(map->spawns[i]));
	}
	map->spawns = NULL;
	
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
			parse_object_begin(&intermediate_map, &node_stack, &node_stack_size, &has_root, &parsed_tiles, array_depth);
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
		case JSON_T_KEY:
			parse_key(value, &intermediate_map, &node_stack, &node_stack_size, parsed_tiles, array_depth);
			break;
		case JSON_T_STRING:
			parse_string(value, &intermediate_map, &node_stack, &node_stack_size, parsed_tiles, array_depth);
			break;
		case JSON_T_INTEGER:
			parse_integer(value, &intermediate_map, &node_stack, &node_stack_size, parsed_tiles, array_depth);
			break;
		case JSON_T_TRUE:
			/* TODO */
			break;
		case JSON_T_FALSE:
			/* TODO */
			break;
	}
	
	return 1;
 }
 
 /*--------------------------------------------------------------------------*/
 /* Parser-Wert, Karte, Tiefenstack, Größe davon, Root-Marker, Anz. geparster Kacheln, Anz. geparster Spawns, Arraytiefe */
 void parse_object_begin(Map* map, unsigned int** stack, unsigned int* stack_size, unsigned int* has_root, int* parsed_tiles,  unsigned int array_depth) {
	if(!(*has_root)) {
		push_node_stack(NODE_ROOT, &stack, stack_size);
		*has_root = 1;
	} else {
		int stack_top = node_stack_at(0, *stack, stack_size);
		int parent = node_stack_at(1, *stack, stack_size);
		
		/* Root->Kacheln */
		if(parent == STACK_ROOT && stack_top == STACK_TILES) {
			if(array_depth == 2 && (map->x * map->y) > 0) {
				(*parsed_tiles)++;
			}
		}
		/* Root->Spawns */
		if(parent == STACK_ROOT && stack_top == STACK_SPAWNS) {
			if(array_depth == 2) {
				map->spawns = (Spawn*)ex_realloc(map->spawns, sizeof(Spawn) * ++map->number_of_spawns);
				map->spawns[map->number_of_spawns - 1] = (Spawn){
						.id = NULL,
						.name = NULL,
						.x = 0,
						.y = 0,
						.direction = WEST,
						.glyph = ' ',
						.color = 0x00000000,
						.npc = 0,
						.humanoid = 0,
						.hp = 0,
						.max_hp = 1,
						.type = 0xFFFFFFFF, // SPAWN_TYPE_INVALID
						.properties = NULL,
						.inventory = NULL,
						.inventory_size = 0
					};
				calculate_spawn_id(&map->spawns[map->number_of_spawns - 1], map->number_of_spawns);
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
	int parent = node_stack_at(1, stack, stack_size);
	
	if(parent == STACK_ROOT && (stack_top == STACK_TILES || stack_top == STACK_SPAWNS)) {
		(*parse_array_depth)++;
	}
 }
 
 /*--------------------------------------------------------------------------*/
 void parse_array_end(unsigned int** stack, unsigned int* stack_size, unsigned int* array_depth) {
	int stack_top = node_stack_at(0, *stack, stack_size);
	int parent = node_stack_at(1, *stack, stack_size);
 
	if(parent == STACK_ROOT && (stack_top == STACK_TILES || stack_top == STACK_SPAWNS)) {
		(*array_depth)--;
		if((*array_depth) == 0) {
			pop_node_stack(stack, stack_size)
		}
	}
 }
 
 /*--------------------------------------------------------------------------*/
 void parse_key(const JSON_value* value, Map* map, unsigned int** stack, unsigned int* stack_size, const int parsed_tiles, const unsigned int array_depth) {
	int stack_top = node_stack_at(0, *stack, stack_size);
	
	/* root (Karte) */
	if(stack_top == STACK_ROOT) {
		/* root->name (Kartenname) */
		if(map->name == NULL && strcmp(value->vu.str.value, NODE_NAME) == 0) {
			push_node_stack(STACK_NAME));
		}
		/* root->x (Breite) */
		else if(strcmp(value->vu.str.value, NODE_X) == 0) {
			push_node_stack(STACK_X);
		}
		/* root->y (Höhe) */
		else if(strcmp(value->vu.str.value, NODE_Y) == 0) {
			push_node_stack(STACK_Y);
		}
		/* root->tiles (Kacheln) */
		else if(map->tiles == NULL && strcmp(value->vu.str.value, NODE_TILES) == 0 && (map->x * map->y > 0)) {
			int i,j;
			push_node_stack(STACK_TILES);
			map->tiles = (Tile*)ex_realloc(map->tiles, sizeof(Tile) * map->x * map->y);
			for(i = 0; i < map->y; ++i) {
				for(j = 0; j < map->x; ++j) {
					int current_index = i * map->x + j;
					map->tiles[current_index] = (Tile){
						.id = NULL,
						.x = i,
						.y = j,
						.spotted = 0,
						.glyph = ' ',
						.color = 0xFFFFFF00,
						.type = TILE_TYPE_INVALID,
						.properties = NULL,
						.items = NULL,
						.number_of_items = 0
					};
					calculate_tile_id(&map->tiles[current_index], i, j);
				}
			}
		}
		/* root->spawns */
		else if(map->spawns == NULL && strcmp(value->vu.str.value, NODE_SPAWNS) == 0) {
			push_node_stack(STACK_SPAWNS);
		}
	}
	/* root->tiles->[] */
	else if(stack_top == STACK_TILES && parse_array_depth == 2) {
		/* tile->type */
		if(map->tiles[parsed_tiles].type == NULL && strcmp(value->vu.str.value, NODE_TYPE) == 0) {
			push_node_stack(STACK_TYPE);
		}
		/* tile->id */
		else if(strcmp(value->vu.str.value, NODE_ID) == 0) {
			push_node_stack(STACK_ID);
		}
		/* tile->items */
		else if(map->tiles[parsed_tiles].items == NULL && strcmp(value->vu.str.value, NODE_ITEMS) == 0) {
			push_node_stack(STACK_ITEMS);
		}
		/* tile->brightness */
		else if(strcmp(value->vu.str.value, NODE_BRIGHTNESS) == 0) {
			push_node_stack(STACK_BRIGHTNESS);
		}
		/* tile->property */
		else {
			int index = tile_property_identifier(value->vu.str.value);
			if(index != STACK_INVALID_INDEX) {
				push_node_stack(index);
			}
		}
	}
	/* root->spawns->[] */
	else if(stack_top == STACK_SPAWNS && parse_array_depth == 2) {
		/* spawn->type */
		if(map->spawns[map->number_of_spawns - 1].type == NULL && strcmp(value->vu.str.value, NODE_TYPE) == 0) {
			push_node_stack(STACK_TYPE);
		}
		/* spawn->id */
		else if(strcmp(value->vu.str.value, NODE_ID) == 0) {
			push_node_stack(STACK_ID);
		}
		/* spawn->name */
		else if(map->spawns[map->number_of_spawns - 1].name == NULL && strcmp(value->vu.str.value, NODE_NAME) == 0) {
			push_node_stack(STACK_NAME);
		}
		/* spawn->direction */
		else if(strcmp(value->vu.str.value, NODE_DIRECTION) == 0) {
			push_node_stack(STACK_NAME);
		}
		/* spawn->hp */
		else if(strcmp(value->vu.str.value, NODE_HEALTHPOINTS) == 0) {
			push_node_stack(STACK_HEALTHPOINTS);
		}
		/* spawn->max_hp */
		else if(strcmp(value->vu.str.value, NODE_MAX_HEALTHPOINTS) == 0) {
			push_node_stack(STACK_MAX_HEALTHPOINTS);
		}
		/* spawn->items */
		else if(map->spawns[map->number_of_spawns - 1].items == NULL && strcmp(value->vu.str.value, NODE_ITEMS) == 0) {
			push_node_stack(NODE_ITEMS);
		}
		/* spawn->property */
		else {
			int index = spawn_property_identifier(value->vu.str.value);
			if(index != STACK_INVALID_INDEX) {
				push_node_stack(index);
			}
		}
	}
	/* TODO: Items */
 }
 
  /*--------------------------------------------------------------------------*/
 void parse_string(const JSON_value* value, Map* map, unsigned int** stack, unsigned int* stack_size, const int parsed_tiles, const unsigned int array_depth) {
	int stack_top = node_stack_at(0, *stack, stack_size);
	int parent = node_stack_at(1, *stack, stack_size);
	
	/* root */
	if(parent == STACK_ROOT) {
		/* root->name (Kartenname) */
		if(stack_top == STACK_NAME) {
			map->name = (char*)ex_calloc(strlen(value->vu.str.value) + 1, 1);
			strcpy(map->name, value->vu.str.value);
		}
	} else if(parent == STACK_TILES) {
		/* verhindert, dass Tiles über die X*Y-Größe geschrieben werden können. */
		if(parsed_tiles >= (map->x) * (map->y)) {
			return;
		}
		
		if(stack_top == STACK_TYPE && map->tiles[parsed_tiles].type == TILE_TYPE_INVALID) {
			int type;
			if((type = tile_type_id(value->vu.str.value)) != TILE_TYPE_INVALID) {
				map->tiles[parsed_tiles].type = type;
				apply_tile_defaults(&map->tiles[parsed_tiles]);
				create_tile_properties(&map->tiles[parsed_tiles]);
			}
		}
	}
 }
 
 /*--------------------------------------------------------------------------*/
 void parse_integer(const JSON_value* value, Map* map, unsigned int** stack, unsigned int* stack_size, const int parsed_tiles, const unsigned int array_depth) {
	int stack_top = node_stack_at(0, *stack, stack_size);
	int parent = node_stack_at(1, *stack, stack_size);
	
	/* root */
	if(parent == STACK_ROOT) {
		/* root->x */
		if(stack_top == STACK_X) {
			map->x = value->vu.integer_value;
		}
		/* root->y */
		else if(stack_top == STACK_Y) {
			map->y = value->vu.integer_value;
		}
	} 
	/* Tile */
	else if(parent == STACK_TILES && array_depth == 2) {
		/* tile->brightness */
		if(stack_top == NODE_BRIGHTNESS) {
			map->tiles[parsed_tiles].brightness = value->vu.integer_value;
		}
	}
	/* Spawn */
	else if(parent == STACK_SPAWNS && array_depth == 2) {
		/* spawn->hp */
		if(stack_top == STACK_HEALTHPOINTS) {
			map->spawns[map->number_of_spawns - 1].hp = value->vu.integer_value;
		}
		/* spawn->max_hp */
		else if(stack_top == STACK_MAX_HEALTHPOINTS) {
			map->spawns[map->number_of_spawns - 1].max_hp = value->vu.integer_value;
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
    node_stack = NULL;
    node_stack_size = 0;
 }
 
 /*--------------------------------------------------------------------------*/
 void calculate_tile_id(Tile* tile, const int x, const int y) {
	/* "tile_","_",0, x als String, y als String */
	int x_length = (int)log10(x) + 1;
	int y_length = (int)log10(y) + 1;
	int id_length = 7 + x_length + y_length;
	char* x_buffer = (char*)ex_calloc(x_length + 1);
	char* y_buffer = (char*)ex_calloc(y_length + 1);
	char* id = (char*)ex_calloc(id_length, 1);
	
	strcpy(id, "tile_"); strcat(id, itoa(x, x_buffer, 10)); strcat(id, "_"); strcat(id, itoa(y, y_buffer, 10));
	tile->id = id;
	free(x_buffer);
	free(y_buffer);
 }
 
 /*--------------------------------------------------------------------------*/
 void calculate_spawn_id(Spawn* spawn, const int number) {
	/* "spawn_",0, number als String */
	int no_length = (int)log10(number) + 1;
	int id_length = 7 + no_length;
	char* no_buffer = (char*)ex_calloc(no_length + 1);
	char* id = (char*)ex_calloc(id_length, 1);
	
	strcpy(id, "spawn_"); strcat(id, itoa(number, no_buffer, 10));
	spawn->id = id;
	free(no_buffer);
 }
 
 /*--------------------------------------------------------------------------*/
 int tile_property_identifier(const char* name) {
	/* kommt noch... */
	return STACK_INVALID_INDEX;
 }
 
 /*--------------------------------------------------------------------------*/
 int spawn_property_identifier(const char* name) {
	/* bestimmt auch... */
	return STACK_INVALID_INDEX;
 }
 
 /*--------------------------------------------------------------------------*/
 unsigned int tile_type_id(const char* name) {
	/* Wand */
	if(strcmp(name, TILE_NAME_WALL) == 0) {
		return TILE_TYPE_WALL;
	}
	/* Boden */
	else if(strcmp(name, TILE_NAME_FLOOR) == 0) {
		return TILE_TYPE_FLOOR;
	}
	/* Button */
	else if(strcmp(name, TILE_NAME_BUTTON) == 0) {
		return TILE_TYPE_BUTTON;
	}
	/* Tür */
	else if(strcmp(name, TILE_NAME_DOOR) == 0) {
		return TILE_TYPE_DOOR;
	}
	return TILE_TYPE_INVALID;
 }
 
 
