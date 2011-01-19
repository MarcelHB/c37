#include "../src/globals.h"
#include "../src/map.h"
#include "../src/map_loader.h"

#define MAP_DIMENSION 10

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

int main(int argc, char** argv) {
	Map* map = NULL;
	
	map = load_map("lonely");
	printf("checking map size...");
	if(map->x != MAP_DIMENSION || map->y != MAP_DIMENSION) {
		printf("\n-> ERROR! expected %dx%d, but map is %dx%d", MAP_DIMENSION, MAP_DIMENSION, map->x, map->y);
	} else { printf("correct!\n"); }
	
	/**
	TODO!!!
	- Doku
	- Stackwirrwarr
	*/
	flush_map(map);
	return 1;
}