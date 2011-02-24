#include "../src/globals.h"
#include "../src/map.h"
#include "../src/map_loader.h"
#include "../src/action.h"

#define MAP_PLAYER_POS1_X 		6
#define MAP_PLAYER_POS1_Y 		3

#define MAP_VIS_TILE_X 			6
#define MAP_VIS_TILE_Y 			2
#define MAP_DARK_TILE_X 		7
#define MAP_DARK_TILE_Y 		2
#define MAP_ITEM_TILE_X 		7
#define MAP_ITEM_TILE_Y 		2
#define MAP_DOOR_TILE_X 		4
#define MAP_DOOR_TILE_Y 		7
#define MAP_BTN1_TILE_X 		8
#define MAP_BTN1_TILE_Y 		3
#define MAP_BTN2_TILE_X 		8
#define MAP_BTN2_TILE_Y 		4
#define MAP_WALL_TILE_X 		3
#define MAP_WALL_TILE_Y 		7
#define MAP_WALL_EXTDOOR_X		2
#define MAP_WALL_EXTDOOR_Y      7

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
	
	/* Interaktionen vom Spieler */
	printf("checking interactions...\n");
	{
		Tile* item_tile = &map->tiles[MAP_ITEM_TILE_Y * map->x + MAP_ITEM_TILE_X];
		Tile* door_tile = &map->tiles[MAP_DOOR_TILE_Y * map->x + MAP_DOOR_TILE_X];
		printf("... moving player to %dx%d and pick up item\n", MAP_ITEM_TILE_X, MAP_ITEM_TILE_Y);
		player->x = MAP_ITEM_TILE_X; player->y = MAP_ITEM_TILE_Y;
		spawn_tile_collision(player, item_tile, map);
		printf("=> checking inventory ...");
		if(player->inventory_size == 1) {
			printf("correct!\n");
		} else { printf("\n-> ERROR! expected 1 but player has %d!\n", player->inventory_size); }
		printf("=> checking tile ...");
		if(item_tile->number_of_items == 0) {
			printf("correct!\n");
		} else { printf("\n-> ERROR! expected 0 but tile has %d!\n", item_tile->number_of_items); }
		printf("... open door at %dx%d,\n", MAP_DOOR_TILE_X, MAP_DOOR_TILE_Y);
		toggle_tile(door_tile, map);
		printf("=> checking state (open) ...");
		if(((DoorProperties*)door_tile->properties)->open == 1) {
			printf("correct!\n");
		} else { printf("\n-> ERROR! door still closed!\n"); }
	}
	
	/* Schalterzusammenhänge */
	printf("checking button actions...\n");
	{
		Tile* btn1_tile = &map->tiles[MAP_BTN1_TILE_Y * map->x + MAP_BTN1_TILE_X];
		Tile* btn2_tile = &map->tiles[MAP_BTN2_TILE_Y * map->x + MAP_BTN2_TILE_X];
		Tile* wall_tile = &map->tiles[MAP_WALL_TILE_Y * map->x + MAP_WALL_TILE_X];
		Tile* door_tile = &map->tiles[MAP_WALL_EXTDOOR_Y * map->x + MAP_WALL_EXTDOOR_X];
		printf("... pushing button at %dx%d,\n", MAP_BTN1_TILE_X, MAP_BTN1_TILE_Y);
		toggle_tile(btn1_tile, map);
		printf("=> checking state of wall (disappeared)...");
		if( ((WallProperties*)wall_tile->properties)->floor ) {
			printf("correct!\n");
		} else { printf("\n-> ERROR! wall is still there!\n"); }
		
		printf("... pushing button at %dx%d,\n", MAP_BTN2_TILE_X, MAP_BTN2_TILE_Y);
		toggle_tile(btn2_tile, map);
		printf("=> checking state of door (opened)...");
		if( ((DoorProperties*)door_tile->properties)->open ) {
			printf("correct!\n");
		} else { printf("\n-> ERROR! door is still closed!\n"); }
	}
	
	flush_map(map);
	return 0;
}