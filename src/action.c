/**
 * C-Projekt von Gruppe 37
 * 
 * action.c
 */

#include <stdlib.h>
#include "globals.h"
#include "SDL.h"
#include "map.h"
#include "spawn.h"
#include "tile.h"
#include "action.h"

/*aktualisiert eine Map gemäß einem Event*/
void process_event(SDL_Event *event, Map *map){
	Spawn *player=get_player_spawn(map);
	Spawn *npc;
	Tile *tile;
	/*Aktion des Spielers*/
	switch(event->key.keysym.sym){
	case SDLK_UP:
		npc=get_spawn_at(player->x,player->y-1);
		player->direction=NORTH;
		/*kein NPC in Laufrichtung*/
		if(npc==NULL){
			/*auf Laufbarem laufen*/
			tile=&(map->tiles[player->x+OUTPUT_IN_GLYPHS_X*(player->y-1)]);
			if(tile_can_walk(*tile)){
				player->y--;
				explore_area(player,map);
			}
			/*mit alles anderem kollidieren*/
			else{
				spawn_tile_collision(player,tile,map,"",0);
			}
		}
		/*NPC in Laufrichtung*/
		else{
			spawn_spawn_collision(player,npc,map);
		}
		break;
	case SDLK_DOWN:
		npc=get_spawn_at(player->x,player->y+1);
		player->direction=SOUTH;
		/*kein NPC in Laufrichtung*/
		if(npc==NULL){
			/*auf Laufbarem laufen*/
			tile=&(map->tiles[player->x+OUTPUT_IN_GLYPHS_X*(player->y+1)]);
			if(tile_can_walk(*tile)){
				player->y++;
				explore_area(player,map);
			}
			/*mit alles anderem kollidieren*/
			else{
				spawn_tile_collision(player,tile,map,"",0);
			}
		}
		/*NPC in Laufrichtung*/
		else{
			spawn_spawn_collision(player,npc,map);
		}
		break;
	case SDLK_LEFT:
		npc=get_spawn_at(player->x-1,player->y);
		/*kein NPC in Laufrichtung*/
		player->direction=WEST;
		if(npc==NULL){
			/*auf Laufbarem laufen*/
			tile=&(map->tiles[player->x-1+OUTPUT_IN_GLYPHS_X*player->y]);
			if(tile_can_walk(*tile)){
				player->x--;
				explore_area(player,map);
			}
			/*mit alles anderem kollidieren*/
			else{
				spawn_tile_collision(player,tile,map,"",0);
			}
		}
		/*NPC in Laufrichtung*/
		else{
			spawn_spawn_collision(player,npc,map);
		}
		break;
	case SDLK_RIGHT:
		npc=get_spawn_at(player->x+1,player->y);
		player->direction=EAST;
		/*kein NPC in Laufrichtung*/
		if(npc==NULL){
			/*auf Laufbarem laufen*/
			tile=&(map->tiles[player->x+1+OUTPUT_IN_GLYPHS_X*player->y]);
			if(tile_can_walk(*tile)){
				player->x++;
				explore_area(player,map);
			}
			/*mit alles anderem kollidieren*/
			else{
				spawn_tile_collision(player,tile,map,"",0);
			}
		}
		/*NPC in Laufrichtung*/
		else{
			spawn_spawn_collision(player,npc,map);
		}
		break;
	case SDLK_SPACE:
		/*zu toggelnden Spawn/Tile feststellen*/
		switch(player->direction){
		case NORTH:
			npc=get_spawn_at(player->x,player->y-1);
			tile=&(map->tiles[player->x+OUTPUT_IN_GLYPHS_X*(player->y-1)]);
			break;
		case SOUTH:
			npc=get_spawn_at(player->x,player->y+1);
			tile=&(map->tiles[player->x+OUTPUT_IN_GLYPHS_X*(player->y+1)]);
			break;
		case WEST:
			npc=get_spawn_at(player->x-1,player->y);
			tile=&(map->tiles[player->x-1+OUTPUT_IN_GLYPHS_X*player->y]);
			break;
		case EAST:
			npc=get_spawn_at(player->x+1,player->y);
			tile=&(map->tiles[player->x+1+OUTPUT_IN_GLYPHS_X*player->y]);
			break;
		}
		/* wenn kein NPC dasteht */
		if(npc == NULL){
			/* checken, ob Button, und ob von hier aus drückbar */
			if(tile->type == TILE_TYPE_BUTTON) {
				ButtonProperties* btn_props = (ButtonProperties*)tile->properties;
				/* von Süden? Player guckt nach Norden */
				if( (player->direction == NORTH && (btn_props->directions & SOUTH)) || 
					(player->direction == SOUTH && (btn_props->directions & NORTH)) ||
					(player->direction == EAST && (btn_props->directions & WEST)) || 
					(player->direction == WEST && (btn_props->directions & EAST)) ) {
					toggle_tile(tile, map);
				}
			} else {
				toggle_tile(tile, map);
			}
		}
		else{
			/*vll noch sowas wie spawn_toggle_spawn?*/
		}
		break;
	default:
		break;
	}
	/*jetzt handeln die NPCs*/
	unsigned int i;
	for(i=0;i<map->number_of_spawns;i++){
		/*den Spieler auslassen*/
		if(map->spawns[i]->npc)
			spawn_action(map->spawns[i]);
	}
}

void spawn_action(Spawn *spawn, Map *map){
	/*soll hier nach typ geswitcht werden?
	 * spawn_run_ai geht atm ja nur für hunde*/
	switch(spawn->type){
	case SPAWN_TYPE_HOUND:
		spawn_run_ai(spawn, map);
		break;
	default:
		break;
	}
}

void
spawn_run_ai (Spawn *self, Map *map) {
    unsigned int nx = self->x, ny = self->y;
    Spawn *player = get_player_spawn(map);
    HoundProperties *props = self->properties;
    if (map_can_see(map, self->x, self->y, player->x, player->y)) {
        props->targetx = player->x;
        props->targety = player->y;
    }
    if (self->x == props->targetx && self->y == props->targety) {
        props->targetx = -1;
        props->targety = -1;
    }

    if ((1 == abs(player->x - self->x) && 0 == abs(player->y - self->y)) ||
        (0 == abs(player->x - self->x) && 1 == abs(player->y - self->y))) {
        /* adjacent, attack */
    } else if (-1 != props->targetx && -1 != props->targety) {
        int dx = props->targetx - self->x, dy = props->targety - self->y;
        if (abs(dx) >= abs(dy)) {
            if (0 < dx) {
                ++nx;
            } else {
                --nx;
            }
        } else {
            if (0 < dy) {
                ++ny;
            } else {
                --ny;
            }
        }
        /* Wir wollen nicht in andere Spawns reinlaufen. */
        Spawn *other = NULL;
        for (unsigned int ii = 0; ii < map->number_of_spawns; ++ii) {
            if (map->spawns[ii]->x == nx && map->spawns[ii]->y == ny) {
                other = map->spawns[ii];
            }
        }
        if (!tile_can_walk(map->tiles[nx * map->y + ny]) || NULL != other) {
            nx = self->x;
            ny = self->y;
        }
    } else {
        int dir = rand() % 4;
        switch (dir) {
            case 0: /* north */
                --ny;
                break;
            case 1: /* east */
                ++nx;
                break;
            case 2: /* south */
                ++ny;
                break;
            case 3: /* west */
                --nx;
                break;
        }
        /* Wir wollen nicht in andere Spawns reinlaufen. */
        Spawn *other = NULL;
        for (unsigned int ii = 0; ii < map->number_of_spawns; ++ii) {
            if (map->spawns[ii]->x == nx && map->spawns[ii]->y == ny) {
                other = map->spawns[ii];
            }
        }
        if (!tile_can_walk(map->tiles[nx * map->y + ny]) || NULL != other) {
            nx = self->x;
            ny = self->y;
        }
    }
    self->x = nx;
    self->y = ny;
}

void spawn_spawn_collision (Spawn *self, Spawn *other, Map *map) {
    if (SPAWN_TYPE_PLAYER == self->type && SPAWN_TYPE_HOUND == other->type) {
        if (other->hp <= 10) {
            Spawn **new_spawns = ex_calloc(map->number_of_spawns - 1, sizeof(Spawn *));
            for (int ii = 0, jj = 0; ii < map->number_of_spawns; ++ii) {
                if (map->spawns[ii] != other) {
                    new_spawns[jj++] = map->spawns[ii];
                }
            }
            free(map->spawns);
            map->spawns = new_spawns;
            --map->number_of_spawns;
            free(other);
        } else {
            other->hp -= 10;
        }
    } else if (SPAWN_TYPE_HOUND == self->type && SPAWN_TYPE_PLAYER == other->type) {
        if (other->hp <= 5) {
            /* Der Spieler ist tot. Irgendwas muss geschehen. */
        } else {
            other->hp -= 5;
        }
    }
}

void spawn_tile_collision (Spawn *self, Tile *tile, Map *map, char **c, int n) {
    if (tile_can_walk(*tile) && tile->number_of_items > 0) {
        /* Items aufsammeln. */
        self->inventory = ex_realloc(self->inventory, self->inventory_size + tile->number_of_items);
        for (int ii = 0; ii < tile->number_of_items; ++ii) {
            self->inventory[self->inventory_size + ii] = tile->items[ii];
        }
        free(tile->items);
        tile->number_of_items = 0;
    } else if (tile->type == TILE_TYPE_WALL) {
        if (self->hp > 0) {
            --self->hp;
            /*Ouch.*/
        }
    }
}

void spawn_uses_item (Spawn *self, Item *item, Map *map) {
    switch (item->type) {
        case ITEM_TYPE_HEALTH_POTION:
            self->hp += ((HealthPotionProperties *)item->properties)->capacity;
            if (self->hp > self->max_hp) {
                self->hp = self->max_hp;
            }
            break;
        default:
            break;
    }
}

void toggle_tile (Tile *self, Map *map) {
	/* Button */
    if(self->type == TILE_TYPE_BUTTON) {	
		ButtonProperties* btn_props = (ButtonProperties*)self->properties;
		if(btn_props->toggle_id == NULL) {
			return;
		}
		unsigned int map_size = map->x * map->y;
		Tile* to_toggle;
		for (unsigned int ii = 0; ii < map_size; ++ii) {
			to_toggle = &map->tiles[ii];
			if(to_toggle->id == NULL || to_toggle == self) {
				continue;
			}
			if (0 == strcmp(btn_props->toggle_id, to_toggle->id)) {
				/* Sonderbehandlung Tür, wenn durch externen Button gedrückt */
				if(to_toggle->type == TILE_TYPE_DOOR) {
					DoorProperties* door_props = (DoorProperties *)to_toggle->properties;
					/* hat externen Schalter, wird also hier getoggelt */
					if(door_props->external_button) {
						door_props->open ^= 1;
					} 
					/* sonst entscheidet der Schalter nur über Verriegelung */
					else {
						door_props->locked ^= 1;
					}
				} else {
					toggle_tile(to_toggle, map);
				}
				break;
			}
		}
	}
	/* Tür */
    else if(self->type == TILE_TYPE_DOOR)  {
		DoorProperties* door_props = (DoorProperties *)self->properties;
		/* Tür hat eigenen Schalter und ist nicht verschlossen */
		if(!(door_props->external_button || door_props->locked)) {
			door_props->open ^= 1;
		}
	}
}
