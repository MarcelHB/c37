/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * map.c
 */

#include "globals.h"
#include "spawn.h"
#include "tile.h"
#include "map.h"

Spawn *
get_player_spawn (Map *self) {
    for (unsigned int ii = 0; ii < self->number_of_spawns; ++ii) {
        if (SPAWN_TYPE_PLAYER == self->spawns[ii]->type) {
            return self->spawns[ii];
        }
    }
    return NULL;
}

/* Überprüft ob eine Stelle auf einer Map von einer anderen aus sichtbar ist.
 * (fx, fy) -> (tx, ty)
 * Siehe: http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html (Step 1) */
bool
map_can_see (Map *ma, unsigned int fx, unsigned int fy, unsigned int tx, unsigned int ty) {
    if (fx >= ma->x || fy >= ma->y ||
        tx >= ma->x || ty >= ma->y ) return false;
    if (fx == tx && fy == ty) return true;
    int dx = tx - fx, dy = ty - fy;
    if (abs(dx) > abs(dy)) {
        float m = (float)dy / (float)dx;
        float b = fy - m * fx;
        dx = (dx < 0) ? -1 : 1;
        while ((dx < 0) ? fx > tx : fx < tx) {
            if (!tile_can_light(ma->tiles[fx * ma->y + (int)(m * fx + b)])) {
                return false;
            }
            fx += dx;
        }
    } else {
        float m = (float)dx / (float)dy;
        float b = fx - m*fy;
        dy = (dy < 0) ? -1 : 1;
        while ((dy < 0) ? fy > ty : fy < ty) {
            if (!tile_can_light(ma->tiles[(int)(m * fy + b) * ma->y + fx])) {
                return false;
            }
            fy += dy;
        }
    }
    return true;
}

/*---------------------------------------------------------------------------*/
Spawn* get_spawn_at(unsigned int x, unsigned int y, Map* map) {
	unsigned int i;
	Spawn* spawn = NULL;
	/* noch in Kartengröße? */
	if(x >= map->x || y >= map->y) {
		return NULL;
	}
	for(i = 0; i < map->number_of_spawns; ++i) {
		Spawn* candidate = map->spawns[i];
		if(candidate->x == x && candidate->y == y) {
			spawn = candidate;
			break;
		}
	}
	return spawn;
}
 
/*---------------------------------------------------------------------------*/
void render_tile(BufferTile* buf, Tile* tile, Map* map) {
	Spawn* spawn = NULL;

	/* noch nicht erkundet */
	if(tile->spotted == 0) {
		return;
	}
	
	spawn = get_spawn_at(tile->x, tile->y, map);
	
	/* Spawn mit Glyphe und Zustandsfarbe rendern */
	if(spawn != NULL) {
		float hp_percentage = (float)spawn->hp / spawn->max_hp;
		unsigned long color = 0;
		/* Zustand: Rotton */
		color = (1 - hp_percentage) * 0xFF;
		/* Zustand: Grünton */
		color = (color << 8) | (unsigned int)((hp_percentage) * 0xFF);
		color <<= 16;
		buf->glyph = spawn->glyph;
		buf->color = color;
	} else {
		/* Item */
		if(tile->number_of_items > 0) {
			buf->glyph = '*';
			buf->color = tile->items[0]->color;
		} else {
			/* sicher ist sicher */
			apply_tile_defaults(tile);
			buf->glyph = tile->glyph;
			buf->color = tile->color;
			
			/* variable Anzeigen */
			/* Tür */
			if(tile->type == TILE_TYPE_DOOR) {
				DoorProperties* door_props = (DoorProperties*)tile->properties;
				/* horizontal? -> geöffnet? */
				if(door_props->horizontal) {
					if(door_props->open) {
						buf->glyph = TILE_GLYPH_DOOR_HORIZ_OPEN;
					} else {
						buf->glyph = TILE_GLYPH_DOOR_HORIZ_CLOSED;
					}
				} else {
					if(door_props->open) {
						buf->glyph = TILE_GLYPH_DOOR_VERT_OPEN;
					} else {
						buf->glyph = TILE_GLYPH_DOOR_VERT_CLOSED;
					}
				}
			}
			/* Wasser */
			else if(tile->type == TILE_TYPE_WATER) {
				/* Wasser färbt sich je nach Tiefe anders blau */
				unsigned int blue = 0x66;
				fprintf(stderr, "water: %c\n", buf->glyph);
				WaterProperties* water_props = (WaterProperties*)tile->properties;
				/* 0.6 ist etwa das richtige, wenn 0->0x66, 255->0xFF gelten soll */
				blue += (water_props->depth * 0.6);
				buf->color = (blue << 8);
			}
		}
	}
	
	/* Helligkeit anpassen */
	{
		unsigned int red, green, blue;
		red = (buf->color >> 24);
		green = (buf->color >> 16) & 0xFF;
		blue = (buf->color >> 8) & 0xFF;
		red *= (tile->brightness / 255.0);
		green *= (tile->brightness / 255.0);
		blue *= (tile->brightness / 255.0);
		buf->color = (((red << 24) | (green << 16)) | (blue << 8));
	}
}

/*---------------------------------------------------------------------------*/
void explore_area(Spawn* spawn, Map* map) {
	int x = spawn->x - (VISUAL_SQUARE/2);
	int y = spawn->y - (VISUAL_SQUARE/2);
	int i, j;
	
	for(i = y; i < y + VISUAL_SQUARE; ++i) {
		for(j = x; j < x + VISUAL_SQUARE; ++j) {
			if(x >= 0 && y >= 0 && x < map->x && y < map->y) {
				map->tiles[i * map->x + j].spotted = 1;
			}
		}
	}
}
