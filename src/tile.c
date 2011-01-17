/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * tile.c
 */
 
 #include "globals.h"
 
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
			tile->properties = malloc(sizeof(WallProperties));
			*(WallProperties*)tile->properties = (WallProperties) {
				.space = 0
			};
		}
		/* Button */
		else if(tile->type == TILE_TYPE_BUTTON) {
			tile->properties = malloc(sizeof(ButtonProperties));
			*(ButtonProperties*)tile->properties = (ButtonProperties) {
				.toggle_id = NULL,
				.once = 0,
				.directions = NORTH | WEST | EAST | SOUTH
			};
		}
		/* Tür */
		else if(tile->type == TILE_TYPE_DOOR) {
			tile->properties = malloc(sizeof(DoorProperties));
			*(DoorProperties*)tile->properties = (DoorProperties) {
				.horizontal = 0,
				.open = 0,
				.external_button = 0,
				.breakable = 0,
				.broken = 0
			};
		}
	}
 }