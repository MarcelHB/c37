#include "../src/globals.h"
#include "../src/map.h"
#include "../src/map_loader.h"

#define MAP_DIMENSION 10
#define MAP_NAME "A lonely map"

const char reference_string[MAP_DIMENSION][MAP_DIMENSION] = {
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
	
	/* Karte->Name */
	printf("checking map name...");
	if(strcmp(MAP_NAME, map->name) != 0) {
		printf("\n-> ERROR! expected %s, but map is %s", MAP_NAME, map->name);
	} else { printf("correct! (%s)\n", map->name); }
	
	/* Karte->Dimension */
	printf("checking map size...");
	if(map->x != MAP_DIMENSION || map->y != MAP_DIMENSION) {
		printf("\n-> ERROR! expected %dx%d, but map is %dx%d", MAP_DIMENSION, MAP_DIMENSION, map->x, map->y);
	} else { printf("correct! (%dx%d)\n", map->x, map->y); }
	
	/**
	TODO!!!
	- Doku
	- Stackwirrwarr
	*/
	flush_map(map);
	return 1;
}