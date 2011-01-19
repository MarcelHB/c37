/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * tile.c
 */
 
 #include "globals.h"
 #include "tile.h"
 
 /*--------------------------------------------------------------------------*/
 void apply_tile_defaults(Tile* tile) {
	int i;
	int elements = sizeof(tile_defaults)/sizeof(tile_defaults[0]);
	
	for(i = 0; i < elements; ++i) {
		if(tile_defaults[i].type == tile->type) {
			tile->color = tile_defaults[i].color;
			tile->glyph = tile_defaults[i].glyph;
			break;
		}
	}
 }
 
 /*--------------------------------------------------------------------------*/
 /* Initialisierungen für typ-spezifische Properties */
 void create_tile_properties(Tile* tile) {
	if(tile->properties == NULL) {
		/* Wand */
		if(tile->type == TILE_TYPE_WALL) {
			WallProperties wall_props = {
				/*.space = */0
			};
			tile->properties = malloc(sizeof(WallProperties));
			*(WallProperties*)tile->properties = wall_props;
		}
		/* Button */
		else if(tile->type == TILE_TYPE_BUTTON) {
			ButtonProperties btn_props = {
				/*.toggle_id = */NULL,
				/*.active = */1,
				/*.once = */0,
				/*.directions = */NORTH | WEST | EAST | SOUTH
			};
			tile->properties = malloc(sizeof(ButtonProperties));
			*(ButtonProperties*)tile->properties = btn_props;
		}
		/* Tür */
		else if(tile->type == TILE_TYPE_DOOR) {
			DoorProperties door_props = {
				/*.horizontal = */0,
				/*.open = */0,
				/*.locked = */0,
				/*.external_button = */0,
				/*.breakable = */0,
				/*.broken = */0
			};
			tile->properties = malloc(sizeof(DoorProperties));
			*(DoorProperties*)tile->properties = door_props;
		}
	}
 }
 
 /*--------------------------------------------------------------------------*/
 void free_tile(Tile* tile) {
	free(tile->id);
	/* Button */
	if(tile->type == TILE_TYPE_BUTTON) {
		/* Button->toggle_id */
		ButtonProperties* btn_props = (ButtonProperties*)tile->properties;
		free(btn_props->toggle_id);
	}
	free(tile->properties);
	free(tile);
 }