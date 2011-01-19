#include "../src/globals.h"
#include "../src/memory.h"
#include "../src/map.h"
#include "../src/map_loader.h"

#define MAP_DIMENSION 			10
#define MAP_NAME 				"A lonely map"
#define MAP_SPAWNS 				1
#define MAP_PLAYER_ID 			"spawn_1"
#define MAP_PLAYER_X 			3
#define MAP_PLAYER_Y 			6
#define MAP_PLAYER_HP 			1000

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
	
	/* Karte->Spawnanzahl */
	printf("checking spawn counter...");
	if(map->number_of_spawns != MAP_SPAWNS) {
		printf("\n-> ERROR! expected %dx, but map has %dx%d!\n", MAP_DIMENSION, map->number_of_spawns);
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
			printf("\n-> ERROR! expected %dx%d, but spawn is %dx%d!\n", MAP_PLAYER_X, MAP_PLAYER_Y, spawn->x, spawn->y);
		} else { printf("correct! (%dx%d)\n", spawn->x, spawn->y); }
		
		/* player->hp */
		printf("=> health points...");
		if(spawn->hp != MAP_PLAYER_HP) {
			printf("\n-> ERROR! expected %dx%d, but spawn has %dx%d!\n", MAP_PLAYER_HP, spawn->hp);
		} else { printf("correct! (%d)\n", spawn->hp); }
	}
	
	flush_map(map);
	return 1;
}