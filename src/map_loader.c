/**
 * C-Projekt von Gruppe 37
 * Autor: Marcel
 * Datum: 14.01.11
 * 
 * map_loader.c
 */
 
 #include "globals.h"
 #include "memory.h"
 #include "map.h"
 #include "map_loader.h"
 
 static int parse(void*, int, const JSON_value*);
 
 /*--------------------------------------------------------------------------*/
 /* JSON_Parser benötigt einen Funktionszeiger als Callback, deshalb nicht über 
  * Parameter lösbar .
  */
 static Map intermediate_map = {
	/*.x = */0, 
	/*.y = */0,
	/*.name = */NULL, 
	/*.tiles = */NULL,
	/*.spawns = */NULL,
	/*.number_of_spawns = */0,
	/*.message_stream = */NULL,
	/*.messages = */0,
	/*.current_message = */0
 };
 static unsigned int array_depth = 0;
 static unsigned int has_root = 0;
 static unsigned int parsed_tiles = 0;
 static unsigned int* node_stack = NULL;
 static unsigned int node_stack_size = 0;
 /* verhindert nachträgliches ändern der Dimensionenen und somit overflows */
 static char dimension_seal = 0;
 
 /*--------------------------------------------------------------------------*/
 /* Lädt die Karte mit dem Namen name in den Speicher und gibt einen Zeiger 
  * darauf zurück. 
  */
 Map* load_map(char* name) {
	FILE* map_file = NULL;
	Map* map = NULL;
	JSON_config config;
	struct JSON_parser_struct* jc = NULL;
	int i;
	char* path_and_name = (char*)ex_calloc(strlen(MAP_DIRECTORY) + strlen(name) + strlen(MAP_FILE_EXTENSION) + 2, 1);
	
	/* "maps/[name].json" */
	sprintf(path_and_name, "%s/%s%s", MAP_DIRECTORY, name, MAP_FILE_EXTENSION);
	
	map_file = fopen(path_and_name, "r");
	/* nix, wenn die Datei fehlt/gesperrt ist */
	if(map_file == NULL) {
		return NULL;
	}
	
	/* modul-statische Variablen zurücksetzen */
	reset_parse_stats();
	
	init_JSON_config(&config);	
	config.depth                  = 6;
    config.callback               = &parse;
    config.allow_comments         = 1;
    config.handle_floats_manually = 0;
	
	jc = new_JSON_parser(&config);
	
	for (i = 0; map_file; ++i) {
        int next_char = fgetc(map_file);
        if (next_char == EOF) {
            break;
        }
		/* Syntaxfehler, unerwartetes Zeichen, Nesting,... */
        if (!JSON_parser_char(jc, next_char)) {
			fprintf(stderr, "JSON_parser_char: syntax error, byte %d, char '%c', error %d\n", i, next_char, JSON_parser_get_last_error(jc));
			clean_up_parsing(jc, path_and_name, map_file, node_stack);
            return NULL;
        }
    }
	
	/* JSON z.B. nicht abgeschlossen */
	if (!JSON_parser_done(jc)) {
		fprintf(stderr, "JSON_parser_end: syntax error\n");
		clean_up_parsing(jc, path_and_name, map_file, node_stack);
		return NULL;
    }
	
	/* kopiert die Zwischenkarte in diesem Scope in einen eigenen Speicherbereich */
	map = finalize_map(map);
	
	clean_up_parsing(jc, path_and_name, map_file, node_stack);
	return map;	
 }
 
 /*--------------------------------------------------------------------------*/
 /* Nimmt eine existierende Karte *map und löscht deeply alle benutzten Ressourcen
  * und die für den Pointer.
  */
 void flush_map(Map* map) {
	unsigned int i;
	unsigned int map_size = map->x * map->y;
	
	/* Name */
	free(map->name);
	
	/* Kacheln */
	for(i = 0; i < map_size; ++i) {
		free_tile(&(map->tiles[i]));
	}
	free(map->tiles);
	map->tiles = NULL;
	
	/* Spawns */
	for(i = 0; i < map->number_of_spawns; ++i) {
		free_spawn(map->spawns[i]);
	}
	free(map->spawns);
	map->spawns = NULL;
	
	/* Messages */
	for(i = 0; i < map->messages; ++i) {
		free(map->message_stream[i]);
	}
	free(map->message_stream);
	
	free(map);
 }
 
 /*--------------------------------------------------------------------------*/
 /* Wenn das Parsen beendet/fehlgeschlagen ist, unlocke die Datei und lösche
  * Parser und Kartennamen.
  */
 void clean_up_parsing(JSON_parser parser, char* name, FILE* map_file, unsigned int* stack) {
	delete_JSON_parser(parser);
	free(stack);
	free(name);
	fclose(map_file);
 }
 
 /*--------------------------------------------------------------------------*/
 /* Callback-Funktion für JSON_parser, liefert den aktuellen semantischen Token 
  * zurück. 
  */
 static int parse(void* cfg, int type, const JSON_value* value) {
    /* Nervkram von C89 */
	(void)cfg;
	
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
			parse_array_begin(&intermediate_map, node_stack, node_stack_size, &array_depth);
			break;
		/* ] */
		case JSON_T_ARRAY_END:
			parse_array_end(&node_stack, &node_stack_size, &array_depth);
			break;
		/* "...": */
		case JSON_T_KEY:
			parse_key(value, &node_stack, &node_stack_size, array_depth);
			break;
		case JSON_T_STRING:
			parse_string(value, &intermediate_map, &node_stack, &node_stack_size, parsed_tiles, array_depth);
			break;
		case JSON_T_INTEGER:
			parse_integer(value, &intermediate_map, &node_stack, &node_stack_size, parsed_tiles, array_depth, &dimension_seal);
			break;
		/* true */
		case JSON_T_TRUE:
			parse_bool(1, &intermediate_map, &node_stack, &node_stack_size, parsed_tiles, array_depth);
			break;
		/* false */
		case JSON_T_FALSE:
			parse_bool(0, &intermediate_map, &node_stack, &node_stack_size, parsed_tiles, array_depth);
			break;
	}
	
	return 1;
 }
 
 /*--------------------------------------------------------------------------*/
 /* Guckt nach, welches Objekt hier beginnt und macht je nach dem ein bisschen mehr.
  */
 void parse_object_begin(Map* map, unsigned int** stack, unsigned int* stack_size, unsigned int* has_root, unsigned int* parsed_tiles,  unsigned int array_depth) {
	/* erstes Objekt muss die Karte sein */
	if(!(*has_root)) {
		push_node_stack(STACK_ROOT, stack, stack_size);
		*has_root = 1;
	} else {
		int stack_top = node_stack_at(0, *stack, *stack_size);
		int parent = node_stack_at(1, *stack, *stack_size);
		
		/* Root->Kacheln */
		if(parent == STACK_ROOT && stack_top == STACK_TILES) {
			if(array_depth == 2 && (map->x * map->y) > 0) {
				(*parsed_tiles)++;
			}
		}
		/* Root */
		/* Spawns werden on the fly angelegt, da ihre Anzahl nicht in direkter Abhängigkeit mit der Kartengröße steht. */
		if(parent == STACK_ROOT) {
			/* Root->spawns */
			if(stack_top == STACK_SPAWNS && map->spawns == NULL) {
				if(array_depth == 1) {
					Spawn spawn = {
							/*.id = */NULL,
							/*.name = */NULL,
							/*.x = */0,
							/*.y = */0,
							/*.direction = */WEST,
							/*.glyph = */' ',
							/*.npc = */0,
							/*.humanoid = */0,
							/*.hp = */0,
							/*.max_hp = */1,
							/*.type = */SPAWN_TYPE_INVALID,
							/*.properties = */NULL,
							/*.inventory = */NULL,
							/*.inventory_size = */0,
							/*.current_item = */0
					};
					map->spawns = (Spawn**)ex_realloc(map->spawns, sizeof(Spawn*) * ++map->number_of_spawns);
					map->spawns[map->number_of_spawns - 1] = (Spawn*)ex_malloc(sizeof(Spawn));
					/* Fallbackwerte */
					*(map->spawns[map->number_of_spawns - 1]) = spawn;
					/* ID abhängig von der Anzahl an Spawns */
					calculate_spawn_id(map->spawns[map->number_of_spawns - 1], map->number_of_spawns);
				}
			}
		}
		/* Tile, Spawn */
		else if(parent == STACK_TILES || parent == STACK_SPAWNS) {
			/* (.)->items */
			if(stack_top == STACK_ITEMS && (parent == STACK_SPAWNS || parent == STACK_TILES)) {
				Item item = {
					/*.id = */NULL,
					/*.name = */NULL,
					/*.color = */0xFFFFFF00,
					/*.weight = */1,
					/*.value = */1,
					/*.type = */ITEM_TYPE_INVALID,
					/*.properties = */NULL
				};
				/* Je nach Owner packe das Item in den entsprechenden Korb */
				if(parent == STACK_SPAWNS) {
					Spawn* spawn = map->spawns[map->number_of_spawns - 1];
					spawn->inventory = (Item**)ex_realloc(spawn->inventory, sizeof(Item*) * ++spawn->inventory_size);
					spawn->inventory[spawn->inventory_size - 1] = (Item*)ex_malloc(sizeof(Item));
					*(spawn->inventory[spawn->inventory_size - 1]) = item;
					calculate_item_id(spawn->inventory[spawn->inventory_size - 1], spawn->id, spawn->inventory_size);
				} else {
					Tile* tile = &map->tiles[*parsed_tiles - 1];
					tile->items = (Item**)ex_realloc(tile->items, sizeof(Item*) * ++tile->number_of_items);
					tile->items[tile->number_of_items - 1] = (Item*)ex_malloc(sizeof(Item));
					*(tile->items[tile->number_of_items - 1]) = item;
					calculate_item_id(tile->items[tile->number_of_items - 1], tile->id, tile->number_of_items);
				}
			}
		}
	}
 }
 
 /*--------------------------------------------------------------------------*/
 /* Objekte, die nicht in einem Array gekapselt sind, werden vom Stack geholt. */
 void parse_object_end(unsigned int** stack, unsigned int* stack_size, unsigned int array_depth) {
	unsigned int stack_top = node_stack_at(0, *stack, *stack_size);
	
	if(stack_top != STACK_INVALID_INDEX && array_depth == 0) {
		pop_node_stack(stack, stack_size);
	}
 }
 
 /*--------------------------------------------------------------------------*/
 /* Bei Array-Beginn wird die aktuelle Array-Tiefe hochgesetzt. */
 void parse_array_begin(Map* map, unsigned int* stack, unsigned int stack_size, unsigned int* array_depth) {
	unsigned int stack_top = node_stack_at(0, stack, stack_size);
	unsigned int parent = node_stack_at(1, stack, stack_size);
	
	/* Root */
	if(parent == STACK_ROOT && (stack_top == STACK_TILES || stack_top == STACK_SPAWNS)) {
		(*array_depth)++;
		/* Root->tiles */
		if(stack_top == STACK_TILES && map->tiles == NULL && (map->x * map->y > 0)) {
			unsigned int i,j;
			map->tiles = (Tile*)ex_realloc(map->tiles, sizeof(Tile) * map->x * map->y);
			for(i = 0; i < map->y; ++i) {
				for(j = 0; j < map->x; ++j) {
					Tile tile = {
						/*.id = */NULL,
						/*.x = */0,
						/*.y = */0,
						/*.spotted = */0,
						/*.brightnes = */0xFF,
						/*.glyph = */' ',
						/*.color = */0xFFFFFF00,
						/*.type = */TILE_TYPE_INVALID,
						/*.properties = */NULL,
						/*.items = */NULL,
						/*.number_of_items = */0
					};
					int current_index = i * map->x + j;
					tile.x = j; tile.y = i;
					map->tiles[current_index] = tile;
					/* Anz. abhängige Kachel-ID */
					calculate_tile_id(&map->tiles[current_index], i, j);
				}
			}
		}
	}
	/* Tile */
	else if(parent == STACK_TILES && stack_top == STACK_ITEMS) {
		(*array_depth)++;
	}
	/* Spawn */
	else if(parent == STACK_SPAWNS && stack_top == STACK_ITEMS) {
		(*array_depth)++;
	}
 }
 
 /*--------------------------------------------------------------------------*/
 /* ... und sonst dekrementiert. Manche Arrays stellen Objektblöcke dar, diese werden
  * dann vom Stack gepustet, wenn zu Ende.
  */
 void parse_array_end(unsigned int** stack, unsigned int* stack_size, unsigned int* array_depth) {
	int stack_top = node_stack_at(0, *stack, *stack_size);
	int parent = node_stack_at(1, *stack, *stack_size);
 
	/* root->tiles/spawns-Array */
	if(parent == STACK_ROOT && (stack_top == STACK_TILES || stack_top == STACK_SPAWNS)) {
		(*array_depth)--;
		if(stack_top == STACK_SPAWNS) {
			pop_node_stack(stack, stack_size);
		}
		else {
			if((*array_depth) == 0) {
				pop_node_stack(stack, stack_size);
			}
		}
	}
	/* tiles/spawns->items-Array */
	else if((parent == STACK_TILES || parent == STACK_SPAWNS) && stack_top == STACK_ITEMS) {
		(*array_depth)--;
		pop_node_stack(stack, stack_size);
	}
 }
 
 /*--------------------------------------------------------------------------*/
 /* Abhängig von übergeordneten Objekten werden alle Strings links vom Doppelpunkt 
  * in JSON hier geprüft und zugeordnet.
  */
 void parse_key(const JSON_value* value, unsigned int** stack, unsigned int* stack_size, const unsigned int array_depth) {
	int stack_top = node_stack_at(0, *stack, *stack_size);
	int parent = node_stack_at(1, *stack, *stack_size);
	
	/* root (Karte) */
	if(stack_top == STACK_ROOT) {
		/* root->name (Kartenname) */
		if(strcmp(value->vu.str.value, NODE_NAME) == 0) {
			push_node_stack(STACK_NAME, stack, stack_size);
			return;
		}
		/* root->x (Breite) */
		else if(strcmp(value->vu.str.value, NODE_X) == 0) {
			push_node_stack(STACK_X, stack, stack_size);
			return;
		}
		/* root->y (Höhe) */
		else if(strcmp(value->vu.str.value, NODE_Y) == 0) {
			push_node_stack(STACK_Y, stack, stack_size);
			return;
		}
		/* root->tiles (Kacheln) */
		/* Kacheln werden (untypisiert) geladen, wenn sich jemand die Mühe gemacht hat, sie überhaupt zu definieren. */
		else if(strcmp(value->vu.str.value, NODE_TILES) == 0) {
			push_node_stack(STACK_TILES, stack, stack_size);
			return;
		}
		/* root->spawns */
		else if(strcmp(value->vu.str.value, NODE_SPAWNS) == 0) {
			push_node_stack(STACK_SPAWNS, stack, stack_size);
			return;
		}
	}
	/* root->tiles->[] */
	else if(stack_top == STACK_TILES && array_depth == 2) {
		/* tile->type */
		if(strcmp(value->vu.str.value, NODE_TYPE) == 0) {
			push_node_stack(STACK_TYPE, stack, stack_size);
			return;
		}
		/* tile->id */
		else if(strcmp(value->vu.str.value, NODE_ID) == 0) {
			push_node_stack(STACK_ID, stack, stack_size);
			return;
		}
		/* tile->items */
		else if(strcmp(value->vu.str.value, NODE_ITEMS) == 0) {
			push_node_stack(STACK_ITEMS, stack, stack_size);
			return;
		}
		/* tile->brightness */
		else if(strcmp(value->vu.str.value, NODE_BRIGHTNESS) == 0) {
			push_node_stack(STACK_BRIGHTNESS, stack, stack_size);
			return;
		}
		/* tile->property */
		else {
			/* Die Erkennung, ob es wirklich irgendein Typ-Property ist, ist ausgelagert. */
			unsigned int index = tile_property_identifier(value->vu.str.value);
			push_node_stack(index, stack, stack_size);
			return;
		}
	}
	/* root->spawns->[] */
	else if(stack_top == STACK_SPAWNS && array_depth == 1) {
		/* spawn->type */
		if(strcmp(value->vu.str.value, NODE_TYPE) == 0) {
			push_node_stack(STACK_TYPE, stack, stack_size);
			return;
		}
		/* spawn->id */
		else if(strcmp(value->vu.str.value, NODE_ID) == 0) {
			push_node_stack(STACK_ID, stack, stack_size);
			return;
		}
		/* spawn->name */
		else if(strcmp(value->vu.str.value, NODE_NAME) == 0) {
			push_node_stack(STACK_NAME, stack, stack_size);
			return;
		}
		/* spawn->x */
		else if(strcmp(value->vu.str.value, NODE_X) == 0) {
			push_node_stack(STACK_X, stack, stack_size);
			return;
		}
		/* spawn->x */
		else if(strcmp(value->vu.str.value, NODE_Y) == 0) {
			push_node_stack(STACK_Y, stack, stack_size);
			return;
		}
		/* spawn->direction */
		else if(strcmp(value->vu.str.value, NODE_DIRECTION) == 0) {
			push_node_stack(STACK_NAME, stack, stack_size);
			return;
		}
		/* spawn->hp */
		else if(strcmp(value->vu.str.value, NODE_HEALTHPOINTS) == 0) {
			push_node_stack(STACK_HEALTHPOINTS, stack, stack_size);
			return;
		}
		/* spawn->max_hp */
		else if(strcmp(value->vu.str.value, NODE_MAX_HEALTHPOINTS) == 0) {
			push_node_stack(STACK_MAX_HEALTHPOINTS, stack, stack_size);
			return;
		}
		/* spawn->items */
		else if(strcmp(value->vu.str.value, NODE_ITEMS) == 0) {
			push_node_stack(STACK_ITEMS, stack, stack_size);
			return;
		}
	}
	/* tile->items->[] */
	else if((stack_top == STACK_ITEMS && parent == STACK_TILES && array_depth == 3) ||
			(stack_top == STACK_ITEMS && parent == STACK_SPAWNS && array_depth == 2)) {
		/* item->type */
		if(strcmp(value->vu.str.value, NODE_TYPE) == 0) {
			push_node_stack(STACK_TYPE, stack, stack_size);
			return;
		}
		/* item->id */
		else if(strcmp(value->vu.str.value, NODE_ID) == 0) {
			push_node_stack(STACK_ID, stack, stack_size);
			return;
		}
		/* item->name */
		else if(strcmp(value->vu.str.value, NODE_NAME) == 0) {
			push_node_stack(STACK_NAME, stack, stack_size);
			return;
		}
		/* item->weight */
		else if(strcmp(value->vu.str.value, NODE_WEIGHT) == 0) {
			push_node_stack(STACK_WEIGHT, stack, stack_size);
			return;
		}
		/* item->value */
		else if(strcmp(value->vu.str.value, NODE_VALUE) == 0) {
			push_node_stack(STACK_VALUE, stack, stack_size);
			return;
		}
		/* item->property */
		else {
			/* Die Erkennung, ob es wirklich irgendein Typ-Property ist, ist ausgelagert. */
			unsigned int index = item_property_identifier(value->vu.str.value);
			push_node_stack(index, stack, stack_size);
			return;
		}
	}
	push_node_stack(STACK_INVALID_INDEX, stack, stack_size);
 }
 
  /*--------------------------------------------------------------------------*/
  /* Wenn auf den Key ein String folgt, gehe entsprechend damit um (wenn vorgesehen) */
 void parse_string(const JSON_value* value, Map* map, unsigned int** stack, unsigned int* stack_size, const unsigned int parsed_tiles, const unsigned int array_depth) {
	unsigned int stack_top = node_stack_at(0, *stack, *stack_size);
	unsigned int parent = node_stack_at(1, *stack, *stack_size);
	(void)array_depth;
	
	/* root */
	if(parent == STACK_ROOT) {
		/* root->name (Kartenname) */
		if(stack_top == STACK_NAME && map->name == NULL) {
			map->name = (char*)ex_calloc(strlen(value->vu.str.value) + 1, 1);
			strcpy(map->name, value->vu.str.value);
		}
	} 
	/* tile */
	else if(parent == STACK_TILES) {
		/* verhindert, dass Tiles über die X*Y-Größe geschrieben werden können. */
		if(parsed_tiles >= (map->x) * (map->y)) {
			return;
		}
		/* tile->type */
		if(stack_top == STACK_TYPE && map->tiles[parsed_tiles-1].type == TILE_TYPE_INVALID) {
			unsigned int type;
			if((type = tile_type_id(value->vu.str.value)) != TILE_TYPE_INVALID) {
				map->tiles[parsed_tiles-1].type = type;
				/* Initialisieren mit Standartwerten */
				apply_tile_defaults(&map->tiles[parsed_tiles-1]);
				/* TypProperty-Zeiger anhängen */
				create_tile_properties(&map->tiles[parsed_tiles-1]);
			}
		}
		/* tile->id */
		else if(stack_top == STACK_ID) {
			/* überschreibt Standard-ID - checkt diese aber nicht auf Kollisionen! */
			if(map->tiles[parsed_tiles-1].id != NULL) {
				free(map->tiles[parsed_tiles-1].id);
			}
			map->tiles[parsed_tiles-1].id = (char*)ex_calloc(strlen(value->vu.str.value) + 1, 1);
			strcpy(map->tiles[parsed_tiles-1].id, value->vu.str.value);
		}
		/* tile->property */
		else {
			/* irgenwas typspezifisches vielleicht */
			parse_string_property(value, map, parent, stack_top, STACK_INVALID_INDEX, parsed_tiles);
		}
	} 
	/* spawn */
	else if(parent == STACK_SPAWNS) {
		Spawn* spawn = map->spawns[map->number_of_spawns-1];
		/* spawn->type */
		if(stack_top == STACK_TYPE && spawn->type == SPAWN_TYPE_INVALID) {
			unsigned int type;
			if((type = spawn_type_id(value->vu.str.value)) != SPAWN_TYPE_INVALID) {
				spawn->type = type;
				apply_spawn_defaults(spawn);
			}
		} 
		/* spawn->id */
		else if(stack_top == STACK_ID) {
			/* überschreibt Standard-ID - checkt diese aber nicht auf Kollisionen! */
			if(spawn->id != NULL) {
				free(spawn->id);
			}
			spawn->id = (char*)ex_calloc(strlen(value->vu.str.value) + 1, 1);
			strcpy(spawn->id, value->vu.str.value);
		}
		/* spawn->name */
		else if(stack_top == STACK_NAME) {
			if(spawn->name != NULL) {
				free(spawn->name);
			}
			spawn->name = (char*)ex_calloc(strlen(value->vu.str.value) + 1, 1);
			strcpy(spawn->name, value->vu.str.value);
		}
	}
	/* item */
	else if(parent == STACK_ITEMS) {
		unsigned int owner = node_stack_at(2, *stack, *stack_size);
		Item* item = NULL;
		/* Je nach Zugehörigkeit Item injecten */
		if(owner == STACK_SPAWNS) {
			Spawn* spawn = map->spawns[map->number_of_spawns - 1];
			item = spawn->inventory[spawn->inventory_size - 1];
		} else {
			Tile* tile = &map->tiles[parsed_tiles - 1];
			item = tile->items[tile->number_of_items - 1];
		}
		/* item->type */
		if(stack_top == STACK_TYPE && item->type == ITEM_TYPE_INVALID) {
			unsigned int type;
			if((type = item_type_id(value->vu.str.value)) != ITEM_TYPE_INVALID) {
				item->type = type;
				/* Initialisieren mit Standartwerten */
				apply_item_defaults(item);
				/* ItemProperty-Zeiger anhängen */
				create_item_properties(item);
			}
		}
		/* item->id */
		else if(stack_top == STACK_ID) {
			/* überschreibt Standard-ID - checkt diese aber nicht auf Kollisionen! */
			if(item->id != NULL) {
				free(item->id);
			}
			item->id = (char*)ex_calloc(strlen(value->vu.str.value) + 1, 1);
			strcpy(item->id, value->vu.str.value);
		}
		/* item->name */
		else if(stack_top == STACK_NAME) {
			if(item->name != NULL) {
				free(item->name);
			}
			item->name = (char*)ex_calloc(strlen(value->vu.str.value) + 1, 1);
			strcpy(item->name, value->vu.str.value);
		}
		/* item->property */
		else {
			/* irgenwas typspezifisches vielleicht */
			parse_string_property(value, map, parent, stack_top, owner, parsed_tiles);
		}
	}
	/* fertig mit Auswertung, springe zurück */
	pop_node_stack(stack, stack_size);
 }
 
 /*--------------------------------------------------------------------------*/
 /* für ein paar Integerwerte dasselbe */
 void parse_integer(const JSON_value* value, Map* map, unsigned int** stack, unsigned int* stack_size, const unsigned int parsed_tiles, const unsigned int array_depth, char* dimension_seal) {
	unsigned int stack_top = node_stack_at(0, *stack, *stack_size);
	unsigned int parent = node_stack_at(1, *stack, *stack_size);
	
	/* root */
	if(parent == STACK_ROOT) {
		/* root->x */
		if(stack_top == STACK_X && (*dimension_seal == 0 || *dimension_seal == 1)) {
			map->x = value->vu.integer_value;
			*dimension_seal |= 2;
		}
		/* root->y */
		else if(stack_top == STACK_Y && (*dimension_seal == 0 || *dimension_seal == 2)) {
			map->y = value->vu.integer_value;
			*dimension_seal |= 1;
		}
	} 
	/* Tile */
	else if(parent == STACK_TILES && array_depth == 2) {
		/* verhindert, dass Tiles über die X*Y-Größe geschrieben werden können. */
		if(parsed_tiles >= (map->x) * (map->y)) {
			return;
		}
		/* tile->brightness */
		if(stack_top == STACK_BRIGHTNESS) {
			map->tiles[parsed_tiles-1].brightness = value->vu.integer_value;
		}
		/* tile->property */
		else {
			parse_integer_property(value, map, parent, stack_top, STACK_INVALID_INDEX, parsed_tiles);
		}
	}
	/* Spawn */
	else if(parent == STACK_SPAWNS && array_depth == 1) {
		/* spawn->hp */
		if(stack_top == STACK_HEALTHPOINTS) {
			map->spawns[map->number_of_spawns - 1]->hp = value->vu.integer_value;
		}
		/* spawn->max_hp */
		else if(stack_top == STACK_MAX_HEALTHPOINTS) {
			map->spawns[map->number_of_spawns - 1]->max_hp = value->vu.integer_value;
		}
		/* spawn->x */
		else if(stack_top == STACK_X) {
			unsigned int x = value->vu.integer_value;
			if(x >= map->x) {
				x = 0;
			}
			map->spawns[map->number_of_spawns - 1]->x = x;
		}
		/* spawn->y */
		else if(stack_top == STACK_Y) {
			unsigned int y = value->vu.integer_value;
			if(y >= map->y) {
				y = 0;
			}
			map->spawns[map->number_of_spawns - 1]->y = y;
		}
	}
	/* item */
	else if(parent == STACK_ITEMS) {
		unsigned int owner = node_stack_at(2, *stack, *stack_size);
		Item* item = NULL;
		/* Je nach Zugehörigkeit Item injecten */
		if(owner == STACK_SPAWNS) {
			Spawn* spawn = map->spawns[map->number_of_spawns - 1];
			item = spawn->inventory[spawn->inventory_size - 1];
		} else {
			Tile* tile = &map->tiles[parsed_tiles - 1];
			item = tile->items[tile->number_of_items - 1];
		}
		/* item->value */
		if(stack_top == STACK_VALUE) {
			item->value = value->vu.integer_value;
		}
		/* item->weight */
		else if(stack_top == STACK_WEIGHT) {
			item->weight = value->vu.integer_value;
		}
		/* item->property */
		else {
			parse_integer_property(value, map, parent, stack_top, owner, parsed_tiles);
		}
	}
	/* fertig mit Auswertung, springe zurück */
	pop_node_stack(stack, stack_size);
 }
 
 /*--------------------------------------------------------------------------*/
 /* JSON unterstüzt true/false, hier meistens in chars übersetzt mit 1/0. */
 void parse_bool(const unsigned int value, Map* map, unsigned int** stack, unsigned int* stack_size, const int unsigned parsed_tiles, const unsigned int array_depth) {
	unsigned int stack_top = node_stack_at(0, *stack, *stack_size);
	unsigned int parent = node_stack_at(1, *stack, *stack_size);
	
	if(parent == STACK_TILES && array_depth == 2) {
		/* verhindert, dass Tiles über die X*Y-Größe geschrieben werden können. */
		if(parsed_tiles >= (map->x) * (map->y)) {
			return;
		}
		/* bisher nur boolesche Properties */
		parse_bool_property(value, map, parent, stack_top, STACK_INVALID_INDEX, parsed_tiles);
	}
	/* fertig mit Auswertung, springe zurück */
	pop_node_stack(stack, stack_size);
 }
 
 /*--------------------------------------------------------------------------*/
 /* Guckt, ob irgendein String-Property sich vom key angesprochen fühlt. */
 void parse_string_property(const JSON_value* value, Map* map, const unsigned int parent, const unsigned int key,  unsigned int owner, const unsigned int parsed_tiles) {
	(void)owner;
	/* tile */
	if(parent == STACK_TILES) {
		unsigned int type = map->tiles[parsed_tiles-1].type;
		if(type == TILE_TYPE_INVALID) {
			return;
		}
		/* Button */
		if(type == TILE_TYPE_BUTTON) {
			ButtonProperties* btn_props = (ButtonProperties*)map->tiles[parsed_tiles-1].properties;
			/* Button->toggle_id */
			if(key == STACK_TOGGLE_ID && btn_props->toggle_id == NULL) {
				btn_props->toggle_id = (char*)ex_calloc(strlen(value->vu.str.value) + 1, 1);
				strcpy(btn_props->toggle_id, value->vu.str.value);
			}
		}
		/* Hinweis */
		else if(type == TILE_TYPE_HINT) {
			HintProperties* hint_props = (HintProperties*)map->tiles[parsed_tiles-1].properties;
			/* Heiweis->Nachricht */
			if(key == STACK_MESSAGE) {
				if(hint_props->message != NULL) {
					free(hint_props->message);
				}
				hint_props->message = (char*)ex_calloc(strlen(value->vu.str.value) + 1, 1);
				strcpy(hint_props->message, value->vu.str.value);
			}
		}
		/* Tür */
		else if(type == TILE_TYPE_DOOR) {
			DoorProperties* door_props = (DoorProperties*)map->tiles[parsed_tiles-1].properties;
			/* Tür->KeyId */
			if(key == STACK_KEY && door_props->key_id == NULL) {
				door_props->key_id = (char*)ex_calloc(strlen(value->vu.str.value) + 1, 1);
				strcpy(door_props->key_id, value->vu.str.value);
			}
		}
	}
 }
 
 /*--------------------------------------------------------------------------*/
 /* Für Integer. */
 void parse_integer_property(const JSON_value* value, Map* map, const unsigned int parent, const unsigned int key, unsigned int owner, const unsigned int parsed_tiles) {
	/* tile */
	if(parent == STACK_TILES) {
		unsigned int type = map->tiles[parsed_tiles-1].type;
		/* Button */
		if(type == TILE_TYPE_BUTTON) {
			ButtonProperties* btn_props = (ButtonProperties*)map->tiles[parsed_tiles-1].properties;
			/* Button->Bedienrichtungen */
			if(key == STACK_DIRECTIONS) {
				btn_props->directions = (char)value->vu.integer_value;
			}
		}
		/* Wasser */
		else if(type == TILE_TYPE_WATER) {
			WaterProperties* water_props = (WaterProperties*)map->tiles[parsed_tiles-1].properties;
			/* Wassertiefe */
			if(key == STACK_DEPTH) {
				water_props->depth = (char)value->vu.integer_value;
			}
		}
	}
	else if(parent == STACK_ITEMS) {
		Item* item = NULL;
		unsigned int type;
		/* Je nach Zugehörigkeit Item injecten */
		if(owner == STACK_SPAWNS) {
			Spawn* spawn = map->spawns[map->number_of_spawns - 1];
			item = spawn->inventory[spawn->inventory_size - 1];
		} else {
			Tile* tile = &map->tiles[parsed_tiles - 1];
			item = tile->items[tile->number_of_items - 1];
		}
		/* Heiltrank */
		type = item->type;
		if(type == ITEM_TYPE_HEALTH_POTION) {
			HealthPotionProperties* hp_props = (HealthPotionProperties*)item->properties;
			/* Heilungskraft */
			if(key == STACK_CAPACITY) {
				hp_props->capacity = (char)value->vu.integer_value;
			}
		}
	}
 }
 
 /*--------------------------------------------------------------------------*/
 /* Für irgendwelche booleschen Eigenschaften. */
 void parse_bool_property(const unsigned int value, Map* map, const unsigned int parent, const unsigned int key, unsigned int owner, const unsigned int parsed_tiles) {
	(void)owner;
	/* tile */
	if(parent == STACK_TILES) {
		int type = map->tiles[parsed_tiles-1].type;
		/* Button */
		if(type == TILE_TYPE_BUTTON) {
			ButtonProperties* btn_props = (ButtonProperties*)map->tiles[parsed_tiles-1].properties;
			/* Button->once */
			if(key == STACK_ONCE) {
				btn_props->once = value;
			}
			/* Button->active */
			else if(key == STACK_ACTIVE) {
				btn_props->active = value;
			}
		}
		/* Wand */
		else if(type == TILE_TYPE_WALL) {
			WallProperties* wall_props = (WallProperties*)map->tiles[parsed_tiles-1].properties;
			/* Wand->begehbar */
			if(key == STACK_SPACE) {
				wall_props->space = value;
			}
		}
		/* Tür */
		else if(type == TILE_TYPE_DOOR) {
			DoorProperties* door_props = (DoorProperties*)map->tiles[parsed_tiles-1].properties;
			/* Tür->horizontal */
			if(key == STACK_HORIZONTAL) {
				door_props->horizontal = value;
			}
			/* Tür->offen? */
			else if(key == STACK_OPEN) {
				door_props->open = value;
			}
			/* Tür->zugeschlossen? */
			else if(key == STACK_LOCKED) {
				door_props->locked = value;
			}
			/* Tür->entfernter Öffner */
			else if(key == STACK_EXTERNAL) {
				door_props->external_button = value;
			}
			/* Tür->eintretbar */
			else if(key == STACK_BREAKABLE) {
				door_props->breakable = value;
			}
		}
	}
}
 
 /*--------------------------------------------------------------------------*/
 /* liefert den internen Identifier vom Stack zurück, 0 ist der Stack-Top, alles größere
  * ist das, was entsprechend weiter zurück liegt */
 unsigned int node_stack_at(unsigned int index, unsigned int* stack, unsigned int stack_size) {
	/* Stack leer oder index außerhalb der Größe */
	if(!stack_size || ((stack_size - index) <= 0)) {
		return STACK_INVALID_INDEX;
	}
	return stack[stack_size - index - 1];
 }
 
 /*--------------------------------------------------------------------------*/
 /* Packt einen Identifier auf den Stack. */
 void push_node_stack(unsigned int node, unsigned int** stack, unsigned int* stack_size) {
	/* Counter hoch */
	unsigned int new_stack_size = ++(*stack_size);
	/* vergrößern */
	*stack = (unsigned int*)ex_realloc(*stack, sizeof(unsigned int) * new_stack_size);
	/* kopieren */
	(*stack)[new_stack_size - 1] = node;
 }
 
 /*--------------------------------------------------------------------------*/
 /* Oder holt ihn wieder da runter. */
 unsigned int pop_node_stack(unsigned int** stack, unsigned int* stack_size) {
	unsigned int tmp;
	unsigned int elements = *stack_size;
	if(!stack_size) {
		return STACK_INVALID_INDEX;
	}
	
	/* temp. Speichern des Top-Elements */
	tmp = (*stack)[elements - 1];
	/* POP */
	elements = --(*stack_size);
	if(!(*stack_size)) {
		free(*stack);
		*stack = NULL;
		return tmp;
	}
	
	/* verkleinern */
	*stack = (unsigned int*)ex_realloc(*stack, sizeof(unsigned int) * elements);
	return tmp;
 }
 
 /*--------------------------------------------------------------------------*/
 /* Die Karte, die in diesem Objectmodul in einer statischen Variable liegt, 
  * wird nun für den Gebrauch einmal kopiert (nur erste Ebene), der Rest wird
  * beim nächsten Parsen eh vergessen.
  */
 Map* finalize_map(Map* map) {
	size_t map_size = sizeof(Map);	
	map = (Map*)ex_malloc(map_size);
	/* bewahre Pointer */
	memcpy(map, &intermediate_map, map_size);
	reset_intermediate_map();	
	return map;
 }
 
 /*--------------------------------------------------------------------------*/
 /* Genauergesagt hier wirds vergessen. Nur benutzen, wenn der reservierte Speicher 
  * für Name, Tiles usw. irgendwo 
  * anders noch referenziert wird! 
  */
 void reset_intermediate_map() {
	intermediate_map.x = 0;
	intermediate_map.y = 0;
	intermediate_map.name = NULL;
	intermediate_map.tiles = NULL;
	intermediate_map.spawns = NULL;
	intermediate_map.number_of_spawns = 0;
	intermediate_map.message_stream = NULL;
	intermediate_map.messages = 0;
	intermediate_map.current_message = 0;
 }
 
 /*--------------------------------------------------------------------------*/
 /* Setzt ein paar interne globale Variablen zurück, die nur beim Parsen wichtig sind */
 void reset_parse_stats() {
	array_depth = 0;
    has_root = 0;
    parsed_tiles = 0;
    node_stack = NULL;
    node_stack_size = 0;
	dimension_seal = 0;
 }
 
 /*--------------------------------------------------------------------------*/
 /* baut aus x und y-Wert eine Standardd-ID für ein Tile */
 void calculate_tile_id(Tile* tile, const unsigned int x, const unsigned int y) {
	/* "tile_","_",0, x als String, y als String */
	int x_length = (int)log10(x + 1) + 1;
	int y_length = (int)log10(y + 1) + 1;
	int id_length = 7 + x_length + y_length;
	char* id_buffer = (char*)ex_calloc(id_length, 1);
	
	/* z.B. "tile_32_54" */
	sprintf(id_buffer, "tile_%d_%d", x + 1, y + 1);
	tile->id = id_buffer;
 }
 
 /*--------------------------------------------------------------------------*/
 /* eine Standardd-ID für einen Spawn, dieses Mal die Nummer */
 void calculate_spawn_id(Spawn* spawn, const unsigned int number) {
	/* "spawn_",0, number als String */
	int no_length = (int)log10(number + 1) + 1;
	int id_length = 7 + no_length;
	char* id_buffer = (char*)ex_calloc(id_length, 1);
	
	/* z.B. "spawn_27" */
	sprintf(id_buffer, "spawn_%d", number);
	spawn->id = id_buffer;
 }
 
 /*--------------------------------------------------------------------------*/
 /* eine Standardd-ID für ein Item, abhängig von der ID des Parents und dessen Nummer */
 void calculate_item_id(Item* item, const char* parent_id, const unsigned int number) {
	int no_length = (int)log10(number + 1) + 1;
	int id_length = strlen(parent_id) + no_length + 7;
	char* id_buffer = (char*)ex_calloc(id_length, 1);
	
	/* z.B. "spawn_27_item_3" */
	sprintf(id_buffer, "%s_item_%d", parent_id, number);
	item->id = id_buffer;
 }
 
 /*--------------------------------------------------------------------------*/
 /* guck nach, ob ein Key zu einem Tile-Property gehört */
 unsigned int tile_property_identifier(const char* name) {
	/* Button->once */
	if(strcmp(name, NODE_ONCE) == 0) {
		return STACK_ONCE;
	}
	/* Button->active */
	else if(strcmp(name, NODE_ACTIVE) == 0) {
		return STACK_ACTIVE;
	}
	/* Button->toggle_id */
	else if(strcmp(name, NODE_TOGGLE_ID) == 0) {
		return STACK_TOGGLE_ID;
	}
	/* Button->directions */
	else if(strcmp(name, NODE_DIRECTIONS) == 0) {
		return STACK_DIRECTIONS;
	}
	/* Wand->imaginär */
	else if (strcmp(name, NODE_SPACE) == 0) {
		return STACK_SPACE;
	}
	/* Tür->horizontal */
	else if (strcmp(name, NODE_HORIZONTAL) == 0) {
		return STACK_HORIZONTAL;
	}
	/* Tür->open */
	else if (strcmp(name, NODE_OPEN) == 0) {
		return STACK_OPEN;
	}
	/* Tür->locked */
	else if (strcmp(name, NODE_LOCKED) == 0) {
		return STACK_LOCKED;
	}
	/* Tür->externer Button */
	else if (strcmp(name, NODE_EXTERNAL) == 0) {
		return STACK_EXTERNAL;
	}
	/* Tür->kaputtbar */
	else if (strcmp(name, NODE_BREAKABLE) == 0) {
		return STACK_BREAKABLE;
	}
	/* Tür->Schlüssel */
	else if (strcmp(name, NODE_KEY) == 0) {
		return STACK_KEY;
	}
	/* Wasser->Tiefe */
	else if (strcmp(name, NODE_DEPTH) == 0) {
		return STACK_DEPTH;
	}
	/* Hinweis->Text */
	else if (strcmp(name, NODE_MESSAGE) == 0) {
		return STACK_MESSAGE;
	}
	return STACK_INVALID_INDEX;
 }
 
 /*--------------------------------------------------------------------------*/
 /* guck nach, ob ein Key zu einem Item-Property gehört */
 unsigned int item_property_identifier(const char* name) {
	/* Heiltrank->HPs */
	if(strcmp(name, NODE_CAPACITY) == 0) {
		return STACK_CAPACITY;
	}
	return STACK_INVALID_INDEX;
 }
  
 /*--------------------------------------------------------------------------*/
 /* ordnet einem Tile-Typenamen aus dem JSON einen internen Identifier zu */
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
	/* Wasser */
	else if(strcmp(name, TILE_NAME_WATER) == 0) {
		return TILE_TYPE_WATER;
	}
	/* Hinweis */
	else if(strcmp(name, TILE_NAME_HINT) == 0) {
		return TILE_TYPE_HINT;
	}
	return TILE_TYPE_INVALID;
 }
 
 /*--------------------------------------------------------------------------*/
 /* ordnet einem Spawn-Typenamen aus dem JSON einen internen Identifier zu */
 unsigned int spawn_type_id(const char* name) {
	/* Player */
	if(strcmp(name, SPAWN_NAME_PLAYER) == 0) {
		return SPAWN_TYPE_PLAYER;
	}
	return SPAWN_TYPE_INVALID;
 }
 
 /*--------------------------------------------------------------------------*/
 /* ordnet einem Item-Typenamen aus dem JSON einen internen Identifier zu */
 unsigned int item_type_id(const char* name) {
	/* Heiltrank */
	if(strcmp(name, ITEM_NAME_HEALTH_POTION) == 0) {
		return ITEM_TYPE_HEALTH_POTION;
	}
	/* Schlüssel */
	else if(strcmp(name, ITEM_NAME_KEY) == 0) {
		return ITEM_TYPE_KEY;
	}
	return ITEM_TYPE_INVALID;
 }
 