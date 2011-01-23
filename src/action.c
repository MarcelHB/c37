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
		/*kein NPC in Laufrichtung*/
		if(npc==NULL){
			/*auf Laufbarem laufen*/
			tile=&(map->tiles[player->x+OUTPUT_IN_GLYPHS_X*(player->y-1)]);
			if(tile_can_walk(*tile)){
				player->y--;
				player->direction=NORTH;
				explore_area(player,map);
			}
			/*mit alles anderem kollidieren*/
			else{
				spawn_tile_collision(player,tile,map,"",0);
				player->direction=NORTH;
			}
		}
		/*NPC in Laufrichtung*/
		else{
			spawn_spawn_collision(player,npc,map);
			player->direction=NORTH;
		}
		break;
	case SDLK_DOWN:
		npc=get_spawn_at(player->x,player->y+1);
		/*kein NPC in Laufrichtung*/
		if(npc==NULL){
			/*auf Laufbarem laufen*/
			tile=&(map->tiles[player->x+OUTPUT_IN_GLYPHS_X*(player->y+1)]);
			if(tile_can_walk(*tile)){
				player->y++;
				player->direction=SOUTH;
				explore_area(player,map);
			}
			/*mit alles anderem kollidieren*/
			else{
				spawn_tile_collision(player,tile,map,"",0);
				player->direction=SOUTH;
			}
		}
		/*NPC in Laufrichtung*/
		else{
			spawn_spawn_collision(player,npc,map);
			player->direction=SOUTH;
		}
		break;
	case SDLK_LEFT:
		npc=get_spawn_at(player->x-1,player->y);
		/*kein NPC in Laufrichtung*/
		if(npc==NULL){
			/*auf Laufbarem laufen*/
			tile=&(map->tiles[player->x-1+OUTPUT_IN_GLYPHS_X*player->y]);
			if(tile_can_walk(*tile)){
				player->x--;
				player->direction=WEST;
				explore_area(player,map);
			}
			/*mit alles anderem kollidieren*/
			else{
				spawn_tile_collision(player,tile,map,"",0);
				player->direction=WEST;
			}
		}
		/*NPC in Laufrichtung*/
		else{
			spawn_spawn_collision(player,npc,map);
			player->direction=WEST;
		}
		break;
	case SDLK_RIGHT:
		npc=get_spawn_at(player->x+1,player->y);
		/*kein NPC in Laufrichtung*/
		if(npc==NULL){
			/*auf Laufbarem laufen*/
			tile=&(map->tiles[player->x+1+OUTPUT_IN_GLYPHS_X*player->y]);
			if(tile_can_walk(*tile)){
				player->x++;
				player->direction=EAST;
				explore_area(player,map);
			}
			/*mit alles anderem kollidieren*/
			else{
				spawn_tile_collision(player,tile,map,"",0);
				player->direction=EAST;
			}
		}
		/*NPC in Laufrichtung*/
		else{
			spawn_spawn_collision(player,npc,map);
			player->direction=EAST;
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
		/*wenn kein NPC dasteht*/
		if(npc==NULL){
			toggle_tile(tile,map);
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
