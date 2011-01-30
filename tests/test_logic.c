#include "../src/globals.h"
#include "../src/map.h"
#include "../src/map_loader.h"

#define MAP_PLAYER_POS1_X 		6
#define MAP_PLAYER_POS1_Y 		3

#define MAP_VIS_TILE_X 			6
#define MAP_VIS_TILE_Y 			2
#define MAP_DARK_TILE_X 		7
#define MAP_DARK_TILE_Y 		2

int main() {
	Map* map = NULL;
	Spawn* player = NULL;
	
	map = load_map("lonely_logic");
	
	if(map == NULL) {
		printf("ERROR! Unable to load file!");
		return 1;
	}
	
	player = get_player_spawn(map);
	
	/* Sichtbarkeit, Aufdeckung */
	printf("checking visibility ...\n");
	{
		printf("... setting %dx%d unspotted\n", MAP_VIS_TILE_X, MAP_VIS_TILE_Y);
		map->tiles[MAP_VIS_TILE_Y * map->x + MAP_VIS_TILE_X].spotted = 0;
		printf("... setting %dx%d dark\n", MAP_DARK_TILE_X, MAP_DARK_TILE_Y);
		map->tiles[MAP_DARK_TILE_Y * map->x + MAP_DARK_TILE_X].brightness = 0;
		printf("... moving player to %dx%d and explore\n", MAP_PLAYER_POS1_X, MAP_PLAYER_POS1_Y);
		player->x = MAP_PLAYER_POS1_X; player->y = MAP_PLAYER_POS1_Y;
		explore_area(player, map);
		printf("=> checking %dx%d as being spotted...", MAP_VIS_TILE_X, MAP_VIS_TILE_Y);
		/* spotting */
		if(map->tiles[MAP_VIS_TILE_Y * map->x + MAP_VIS_TILE_X].spotted) {
			printf("correct!\n");
		} else { printf("\n-> ERROR! not spotted!\n"); };
		/* brightness */
		printf("=> checking %dx%d as too dark...", MAP_DARK_TILE_X, MAP_DARK_TILE_Y);
		if(!map->tiles[MAP_DARK_TILE_X * map->x + MAP_DARK_TILE_Y].spotted) {
			printf("correct!\n");
		} else { printf("\n-> ERROR! spotted!\n"); };
	}
	
	flush_map(map);
	return 0;
}