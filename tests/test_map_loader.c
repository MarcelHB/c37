#include "../src/globals.h"
#include "../src/memory.h"
#include "../src/map.h"
#include "../src/map_loader.h"

#define MAP_DIMENSION 			10
#define MAP_NAME 				"A lonely map"
#define MAP_SPAWNS 				1
#define MAP_ITEM_X 				7
#define MAP_ITEM_Y 				2
#define MAP_ITEM_VALUE 			100
#define MAP_ITEM_WEIGHT			10
#define MAP_ITEM_CAPACITY		100
#define MAP_PLAYER_ID 			"spawn_1"
#define MAP_PLAYER_X 			3
#define MAP_PLAYER_Y 			6
#define MAP_PLAYER_HP 			1000
#define MAP_PLAYER_ITEMS		2

const char glyph_reference[MAP_DIMENSION][MAP_DIMENSION] = {
	{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
	{' ', '#', '#', '#', '#', '#', '#', '#', '#', ' '},
	{' ', '#', '.', '.', '.', '.', '.', '.', '#', ' '},
	{' ', '#', '.', '.', '.', '.', '.', '.', '#', ' '},
	{' ', '#', '.', '.', '.', '.', '.', '.', '#', ' '},
	{' ', '#', '.', '.', '.', '.', '.', '.', '#', ' '},
	{' ', '#', '.', '.', '.', '.', '.', '.', '#', ' '},
	{' ', '#', '.', '.', '.', '.', '.', '.', '#', ' '},
	{' ', '#', '#', '#', '#', '#', '#', '#', '#', ' '},
	{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
};

int main() {
	Map* map = NULL;
	
	map = load_map("lonely");
	
	if(map == NULL) {
		printf("ERROR! Unable to load file!");
		return 1;
	}
	
	/* Karte->Name */
	printf("checking map name...");
	if(strcmp(MAP_NAME, map->name) != 0) {
		printf("\n-> ERROR! expected %s, but map is %s\n", MAP_NAME, map->name);
	} else { printf("correct! (%s)\n", map->name); }
	
	/* Karte->Dimension */
	printf("checking map size...");
	if(map->x != MAP_DIMENSION || map->y != MAP_DIMENSION) {
		printf("\n-> ERROR! expected %dx%d, but map is %dx%d\n", MAP_DIMENSION, MAP_DIMENSION, map->x, map->y);
	} else { printf("correct! (%dx%d)\n", map->x, map->y); }
	
	/* Karte->Icons (Types) */
	printf("checking glyphs...\n");
	{
		unsigned int i, j;
		char glyph_error = 0;
		for(i = 0; i < map->y; ++i) {
			for(j = 0; j < map->x; ++j) {
				printf("%c", map->tiles[i * map->x + j].glyph);
				if(map->tiles[i * map->x + j].glyph != glyph_reference[i][j]) {
					glyph_error = 1;
					break;
				}
			}
			if(glyph_error) break;
			printf("\n");
		}
		if(glyph_error) {
			printf("\n-> ERROR! glyphs do not match at %dx%d!\n", j, i);
			return 1;
		} else {
			printf("correct!\n");
		}
	}
	
	/* Karte->tiles[]->id (Types) */
	printf("checking diagonal ids...");
	{
		int i;
		char* id_buffer;
		char id_error = 0;
		
		for(i = 0; i < MAP_DIMENSION; ++i) {
			id_buffer = (char*)ex_calloc(9, 1);
			sprintf(id_buffer, "tile_%d_%d", i+1, i+1);
			if(strcmp(id_buffer, map->tiles[i * map->x + i].id) != 0) {
				id_error = 1;
				free(id_buffer);
				break;
			}
			free(id_buffer);
		}
		
		if(id_error) {
			printf("\n-> ERROR! ids do not match at %dx%d!\n", i, i);
		} else {
			printf("correct!\n");
		}
	}
	
	/* Karte->tiles[x]->items[] */
	printf("checking special potion at 7x2...");
	{
		Tile* tile = &map->tiles[MAP_ITEM_Y * map->x + MAP_ITEM_X];
		if(tile->number_of_items == 1) {
			Item* item = tile->items[0];
			int item_error = 0;
			char* id_buffer = NULL;
			/* item->type */
			if(item->type != ITEM_TYPE_HEALTH_POTION) {
				printf("\n-> ERROR! item is not a health potion! (is: %d)", item->type);
				item_error = 1;
			}
			/* item->id */
			id_buffer = (char*)ex_calloc(strlen(tile->id) + 8, 1);
			sprintf(id_buffer, "%s_item_1", tile->id);
			if(strcmp(id_buffer, item->id) != 0) {
				printf("\n-> ERROR! expected id tile_8_3_item_1, but item is %s!", item->id);
				item_error = 1;
			}
			free(id_buffer);
			/* item->value */
			if(item->value != MAP_ITEM_VALUE) {
				printf("\n-> ERROR! expected value %d, but item has %d!", MAP_ITEM_VALUE, item->value);
				item_error = 1;
			}
			/* item->weight */
			if(item->weight != MAP_ITEM_WEIGHT) {
				printf("\n-> ERROR! expected weight %d, but item has %d!", MAP_ITEM_WEIGHT, item->weight);
				item_error = 1;
			}
			/* item->capacity */
			{
				HealthPotionProperties* hpps = (HealthPotionProperties*)item->properties;
				if(hpps->capacity != MAP_ITEM_CAPACITY) {
					printf("\n-> ERROR! expected capcity %d, but item has %d!", MAP_ITEM_CAPACITY, hpps->capacity);
					item_error = 1;
				}
			}
			
			if(!item_error) {
				printf("correct!\n"); 
			} else {
				printf("\n"); 
			}
		} else {
			printf("\n-> ERROR! expected %d item, but tile has %d!\n", 1, tile->number_of_items);
		}
	}
	
	/* Karte->Spawnanzahl */
	printf("checking spawn counter...");
	if(map->number_of_spawns != MAP_SPAWNS) {
		printf("\n-> ERROR! expected %dx, but map has %dx!\n", MAP_DIMENSION, map->number_of_spawns);
	} else { printf("correct! (%d)\n", map->number_of_spawns); }
	
	printf("checking vanilla player...\n");
	{
		Spawn* spawn = map->spawns[0];
		/* player->id */
		printf("=> id...");
		if(strcmp(MAP_PLAYER_ID, spawn->id) != 0) {
			printf("\n-> ERROR! expected %s but player is %s!\n", MAP_PLAYER_ID, spawn->id);
		} else { printf("correct! (%s)\n", spawn->id); }
		
		/* player->type */
		printf("=> type...");
		if(spawn->type != SPAWN_TYPE_PLAYER) {
			printf("\n-> ERROR! spawn at 0 is not a player type!\n");
		} else { printf("correct! (SPAWN_TYPE_PLAYER)\n"); }
		
		/* player->position */
		printf("=> position...");
		if(spawn->x != MAP_PLAYER_X || spawn->y != MAP_PLAYER_Y) {
			printf("\n-> ERROR! expected %dx%d, but player is %dx%d!\n", MAP_PLAYER_X, MAP_PLAYER_Y, spawn->x, spawn->y);
		} else { printf("correct! (%dx%d)\n", spawn->x, spawn->y); }
		
		/* player->hp */
		printf("=> health points...");
		if(spawn->hp != MAP_PLAYER_HP) {
			printf("\n-> ERROR! expected %dx, but player has %dx!\n", MAP_PLAYER_HP, spawn->hp);
		} else { printf("correct! (%d)\n", spawn->hp); }
		
		/* player->items */
		printf("=> two standard health potions in inventory...");
		if(spawn->inventory_size == 2) {
			int i;
			int item_error = 0;
			for(i = 0; i < MAP_PLAYER_ITEMS; ++i) {
				Item* item = spawn->inventory[i];
				char* id_buffer = NULL;
				/* item->type */
				if(item->type != ITEM_TYPE_HEALTH_POTION) {
					item_error = 1; break;
				}
				/* item->id */
				id_buffer = (char*)ex_calloc(strlen(spawn->id) + 8, 1);
				sprintf(id_buffer, "%s_item_%d", spawn->id, i+1);
				if(strcmp(id_buffer, item->id) != 0) {
					free(id_buffer); item_error = 1; break;
				}
				free(id_buffer);
			}
			if(item_error) {
				printf("\n-> ERROR! item %d does not seem to be a valid health potion!\n", i);
			} else { printf("correct!\n"); }
		} else {
			printf("\n-> ERROR! expected %d items, but player has %dx!\n", MAP_PLAYER_ITEMS, spawn->inventory_size);
		}
	}
	
	flush_map(map);
	return 0;
}