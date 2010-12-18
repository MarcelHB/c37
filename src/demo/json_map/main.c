#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "JSON_parser.h"

//-----------------------------------------------------------------------------
static int parse(void* ctx, int type, const JSON_value* value);
void push_node_stack(char*);
char* pop_node_stack();
char* stack_at(unsigned int);

void* tile_create_properties(char*);

void tile_string_property(int, char*);
void tile_bool_property(int, int);

void show_map();
void free_map();

//-----------------------------------------------------------------------------
typedef enum { HORIZ, VERT } Directions;

typedef struct WallProperties {
	Directions direction;
	int space;
} WallProperties;

typedef struct Tile {
	unsigned int x,y;
	char glyph;
	char* type;
	void* properties;
} Tile;

typedef struct Map {
	unsigned int x, y;
	char* name;
	Tile* tiles;
} Map;

//-----------------------------------------------------------------------------
int parse_root_found = 0;
int parse_array_depth = 0;
int parse_map_tile = -1;

char** node_stack = NULL;
int stack_size = 0;

// keys
char* node_name_root = "root";
char* node_name_root_name = "name";
char* node_name_root_x = "x";
char* node_name_root_y = "y";
char* node_name_root_tiles = "tiles";
char* node_name_tile_type = "type";

// tile types
char* tile_type_wall = "wall";
char* tile_type_space = "floor";
char* tile_types[2] = {"wall", "floor"};

// tile properties
char* tile_wall_direction = "direction";
char* tile_wall_direction_vert = "V";
char* tile_wall_space = "floor";

Map map = {
	.x = 0, 
	.y = 0,
	.name = NULL, 
	.tiles = NULL
};

//-----------------------------------------------------------------------------
int main(int argc, char** argv) {
	FILE* map_file;
	JSON_config config;
	struct JSON_parser_struct* jc = NULL;
	int i;
	
	if(argc <= 1) {
		printf("ERROR: Please give a json file!\n");
		return 1;
	}
	
	map_file = fopen(argv[1], "r");
	if(map_file == NULL) {
		printf("ERROR: Could not open desired file!\n");
		return 1;
	}
	
	init_JSON_config(&config);
	config.depth                  = 19;
    config.callback               = &parse;
    config.allow_comments         = 1;
    config.handle_floats_manually = 0;
	
	jc = new_JSON_parser(&config);
	
	for (i = 0; map_file; ++i) {
        int next_char = fgetc(map_file);
        if (next_char == EOF) {
            break;
        }
        if (!JSON_parser_char(jc, next_char)) {
            printf("JSON_parser_char: syntax error, byte %d\n", i);
			delete_JSON_parser(jc);
            return 1;
        }
    }	
	fclose(map_file);
	
	if (!JSON_parser_done(jc)) {
        printf("JSON_parser_end: syntax error\n");
		delete_JSON_parser(jc);
		return 1;
    }
	
	show_map();

	node_stack = NULL;
	free_map();
	return 0;
}

//-----------------------------------------------------------------------------
static int parse(void* ctx, int type, const JSON_value* value) {
	char* stack_top = stack_at(0);
	
	switch(type) {
		case JSON_T_OBJECT_BEGIN:
			// first element should be the root object, the map
			if(!parse_root_found) {
				push_node_stack(node_name_root);
				parse_root_found = 1;
			} else {
				if(stack_top != NULL && strcmp(stack_top, node_name_root_tiles) == 0) {
					if(parse_array_depth == 2 && (map.x * map.y) > 0) {
						parse_map_tile++;
					}
				}
			}
			break;
		case JSON_T_OBJECT_END:
			if(stack_top != NULL && strcmp(stack_top, node_name_root) == 0 && parse_array_depth == 0) {
				free(pop_node_stack());
			}
			break;
		case JSON_T_ARRAY_BEGIN:
			if(stack_top != NULL && strcmp(stack_top, node_name_root_tiles) == 0) {
				parse_array_depth++;
			}
			break;
		case JSON_T_ARRAY_END:
			if(stack_top != NULL && strcmp(stack_top, node_name_root_tiles) == 0) {
				parse_array_depth--;
			}
			if(stack_top != NULL && strcmp(stack_top, node_name_root_tiles) == 0 && parse_array_depth == 0) {
				free(pop_node_stack());
			}
			break;
		case JSON_T_KEY:
			// root keys
			if(stack_top != NULL && strcmp(stack_top, node_name_root) == 0) {
				// root -> name
				if(strcmp(value->vu.str.value, node_name_root_name) == 0) {
					push_node_stack(node_name_root_name);
				} else if(strcmp(value->vu.str.value, node_name_root_x) == 0) {
				// root -> x
					push_node_stack(node_name_root_x);
				} else if(strcmp(value->vu.str.value, node_name_root_y) == 0) {
				// root -> y
					push_node_stack(node_name_root_y);
				} else if(strcmp(value->vu.str.value, node_name_root_tiles) == 0) {
				// root -> tiles
					int i,j;
					push_node_stack(node_name_root_tiles);
					map.tiles = realloc(map.tiles, sizeof(Tile) * map.x * map.y);
					for(i = 0; i < map.y; ++i) {
						for(j = 0; j < map.x; ++j) {
							map.tiles[i * map.x + j] = (Tile){
								.x = i,
								.y = j,
								.glyph = ' ',
								.type = NULL,
								.properties = NULL
							};
						}
					}
				}
			// tiles
			} else if(	stack_top != NULL && 
						strcmp(stack_top, node_name_root_tiles) == 0 && 
						parse_array_depth == 2	) {
				// tile -> type
				if(strcmp(value->vu.str.value, node_name_tile_type) == 0) {
					push_node_stack(node_name_tile_type);
				} else {
				// tile -> any property
					push_node_stack((char*)value->vu.str.value);
				}
			}
			break;
		case JSON_T_STRING:
			// root
			if(stack_at(1) != NULL && strcmp(stack_at(1), node_name_root) == 0) {	
				// root -> (name)
				if(strcmp(stack_top, node_name_root_name) == 0) {
					map.name = realloc(map.name, strlen(value->vu.str.value) + 1);
					memset(map.name, 0, strlen(value->vu.str.value) + 1);
					memcpy(map.name, value->vu.str.value, strlen(value->vu.str.value));
				}
			// tiles
			} else if(	stack_at(1) != NULL && 
						strcmp(stack_at(1), node_name_root_tiles) == 0 &&
						parse_array_depth == 2 ) {	
				// tile -> type
				if(parse_map_tile >= (map.x) * (map.y)) {
					break;
				}
				if(strcmp(stack_top, node_name_tile_type) == 0) {
					int i;
					for(i = 0; i < (sizeof(tile_types)/sizeof(tile_types[0])); ++i) {
						if(strcmp(tile_types[i], value->vu.str.value) == 0) {
							map.tiles[parse_map_tile].type = realloc(map.tiles[parse_map_tile].type, strlen(value->vu.str.value) + 1);
							memset(map.tiles[parse_map_tile].type, 0, strlen(value->vu.str.value) + 1);
							memcpy(map.tiles[parse_map_tile].type, value->vu.str.value, strlen(value->vu.str.value));
							
							map.tiles[parse_map_tile].properties = tile_create_properties((char*)value->vu.str.value);
							map.tiles[parse_map_tile].glyph = '.'; // will be overriden
							break;
						}
					}
				// tile -> (any property)
				} else {
					tile_string_property(parse_map_tile, (char*)value->vu.str.value);
				}
			}
			free(pop_node_stack());
			break;
		case JSON_T_INTEGER:
			// root
			if(stack_at(1) != NULL && strcmp(stack_at(1), node_name_root) == 0) {	
				// root -> (x)
				if(strcmp(stack_top, node_name_root_x) == 0) {
					map.x = value->vu.integer_value;
				} else if(strcmp(stack_top, node_name_root_y) == 0) {
				// root -> (y)
					map.y = value->vu.integer_value;
				}
			}
			free(pop_node_stack());
			break;
		case JSON_T_TRUE:
			if(	stack_at(1) != NULL && 
				strcmp(stack_at(1), node_name_root_tiles) == 0 &&
				parse_array_depth == 2 ) {
				tile_bool_property(parse_map_tile, 1);
			}
			free(pop_node_stack());
			break;
		case JSON_T_FALSE:
			if(	stack_at(1) != NULL && 
				strcmp(stack_at(1), node_name_root_tiles) == 0 &&
				parse_array_depth == 2 ) {
				tile_bool_property(parse_map_tile, 0);
			}
			free(pop_node_stack());
			break;
	}
		
	return 1;
}

//-----------------------------------------------------------------------------
void push_node_stack(char* node) {
	node_stack = realloc(node_stack, sizeof(char*) * ++stack_size);
	node_stack[stack_size-1] = (char*)malloc(strlen(node) + 1);
	memset(node_stack[stack_size-1], 0, strlen(node) + 1);
	memcpy(node_stack[stack_size-1], node, strlen(node));
}

//-----------------------------------------------------------------------------
char* stack_at(unsigned int i) {
	if(!stack_size || ((stack_size-i) <= 0)) {
		return (char*)NULL;
	}
	return node_stack[stack_size - 1 - i];
}

//-----------------------------------------------------------------------------
char* pop_node_stack() {
	char* tmp;
	if(!stack_size) {
		return (char*)NULL;
	}
	
	tmp = (char*)malloc(strlen(node_stack[stack_size-1]) + 1);
	memset(tmp, 0,  strlen(node_stack[stack_size-1]));
	memcpy(tmp, node_stack[stack_size-1], strlen(node_stack[stack_size-1]));
	
	free(node_stack[stack_size-1]);
	--stack_size;
	
	if(!stack_size) {
		return (char*)NULL;
	}
	
	node_stack = realloc(node_stack, sizeof(char*) * stack_size);
	return tmp;
}

//-----------------------------------------------------------------------------
void* tile_create_properties(char* type) {
	void* properties = NULL;
	if(type == NULL) {
		return NULL;
	}
	// walls
	if(strcmp(type, tile_type_wall) == 0) {
		properties = malloc(sizeof(WallProperties));
		*(WallProperties*)properties = (WallProperties) {
			.direction = HORIZ,
			.space = 0
		};
	}
	
	return properties;
}

//-----------------------------------------------------------------------------
void tile_string_property(int index, char* value) {
	char* stack_top = stack_at(0);
	
	if(map.tiles == NULL || map.tiles[index].properties == NULL || parse_map_tile >= (map.x) * (map.y)) {
		return;
	}
	// walls
	if(strcmp(map.tiles[index].type, tile_type_wall) == 0) {
		WallProperties* properties = (WallProperties*)map.tiles[index].properties;
		// wall -> direction
		if(strcmp(stack_top, tile_wall_direction) == 0) {
			// wall -> direction -> "V" ?
			if(strcmp(value, tile_wall_direction_vert) == 0) {
				properties->direction = VERT;
				map.tiles[index].glyph = '|';
			} else {
				map.tiles[index].glyph = '_';
			}
		}
	}
}

//-----------------------------------------------------------------------------
void tile_bool_property(int index, int value) {
	char* stack_top = stack_at(0);
	
	if(map.tiles == NULL || map.tiles[index].properties == NULL || parse_map_tile >= (map.x) * (map.y)) {
		return;
	}
	// walls
	if(strcmp(map.tiles[index].type, tile_type_wall) == 0) {
		WallProperties* properties = (WallProperties*)map.tiles[index].properties;
		// wall -> space?
		if(strcmp(stack_top, tile_wall_space) == 0) {
			properties->space = value;
		}
	}
}

//-----------------------------------------------------------------------------
void show_map() {
	int i, j;
	printf("Showing '%s' (%dx%d):\n\n", map.name, map.x, map.y);
	
	if(map.tiles == NULL) {
		return;
	}
	
	for(i = 0; i < map.y; ++i) {
		for(j = 0; j < map.x; ++j) {
			printf("%c", map.tiles[i * map.x + j].glyph);
		}
		
		printf("\n");
	}
}

//-----------------------------------------------------------------------------
void free_map() {
	int i;
	
	free(map.name);
	if(map.tiles == NULL) {
		return;
	}
	
	for(i = 0; i < map.x * map.y; ++i) {
		free(map.tiles[i].type);
		
		if(map.tiles[i].properties == NULL) {
			continue;
		}
		free(map.tiles[i].properties);
	}
}