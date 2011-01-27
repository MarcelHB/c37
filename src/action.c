/**
 * C-Projekt von Gruppe 37
 * 
 * action.c
 */

#include <stdlib.h>
#include "globals.h"
#include "memory.h"
#include "SDL.h"
#include "map.h"
#include "spawn.h"
#include "tile.h"
#include "action.h"
#include "sdl_output.h"

/*Helper, löscht Item*/
static int delete_item(Item*, Spawn*);

/*aktualisiert eine Map gemäß einem Event*/
void process_event(SDL_Event *event, Map *map){
	Spawn *player=get_player_spawn(map);
	Spawn *npc;
	Tile *tile;
	/*Aktion des Spielers*/
	switch(event->key.keysym.sym){
	case SDLK_UP:
		npc=get_spawn_at(player->x,player->y-1, map);
		player->direction=NORTH;
		/*kein NPC in Laufrichtung*/
		if(npc==NULL){
			/*auf Laufbarem laufen*/
			tile=&(map->tiles[player->x+map->x*(player->y-1)]);
			player->y--;
			spawn_tile_collision(player,tile,map);
			explore_area(player,map);
		}
		/*NPC in Laufrichtung*/
		else{
			spawn_spawn_collision(player,npc,map);
		}
		break;
	case SDLK_DOWN:
		npc=get_spawn_at(player->x,player->y+1, map);
		player->direction=SOUTH;
		/*kein NPC in Laufrichtung*/
		if(npc==NULL){
			/*auf Laufbarem laufen*/
			tile=&(map->tiles[player->x+map->x*(player->y+1)]);
			player->y++;
			spawn_tile_collision(player,tile,map);
			explore_area(player,map);
		}
		/*NPC in Laufrichtung*/
		else{
			spawn_spawn_collision(player,npc,map);
		}
		break;
	case SDLK_LEFT:
		npc=get_spawn_at(player->x-1,player->y, map);
		/*kein NPC in Laufrichtung*/
		player->direction=WEST;
		if(npc==NULL){
			/*auf Laufbarem laufen*/
			tile=&(map->tiles[player->x-1+map->x*player->y]);
			player->x--;
			spawn_tile_collision(player,tile,map);
			explore_area(player,map);
		}
		/*NPC in Laufrichtung*/
		else{
			spawn_spawn_collision(player,npc,map);
		}
		break;
	case SDLK_RIGHT:
		npc=get_spawn_at(player->x+1,player->y, map);
		player->direction=EAST;
		/*kein NPC in Laufrichtung*/
		if(npc==NULL){
			/*auf Laufbarem laufen*/
			tile=&(map->tiles[player->x+1+map->x*player->y]);
			player->x++;
			spawn_tile_collision(player,tile,map);
			explore_area(player,map);
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
			npc=get_spawn_at(player->x,player->y-1, map);
			tile=&(map->tiles[player->x+map->x*(player->y-1)]);
			break;
		case SOUTH:
			npc=get_spawn_at(player->x,player->y+1, map);
			tile=&(map->tiles[player->x+map->x*(player->y+1)]);
			break;
		case WEST:
			npc=get_spawn_at(player->x-1,player->y, map);
			tile=&(map->tiles[player->x-1+map->x*player->y]);
			break;
		case EAST:
			npc=get_spawn_at(player->x+1,player->y, map);
			tile=&(map->tiles[player->x+1+map->x*player->y]);
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
	case SDLK_PAGEUP:
		/*Inventar durchschalten*/
		if(player->inventory!=NULL && player->inventory_size!=0){
			player->selected_item=(player->selected_item+1)%player->inventory_size;
			update_item(player->inventory[player->selected_item]->name, player->selected_item+1);
		}
		break;
	case SDLK_PAGEDOWN:
		if(player->inventory!=NULL && player->inventory_size!=0){
			if(player->selected_item)
				player->selected_item=player->selected_item-1;
			else
				player->selected_item=player->inventory_size-1;
			update_item(player->inventory[player->selected_item]->name, player->selected_item+1);
		}
		break;
	case SDLK_INSERT:
		/*durch die History scrollen, nicht wrappen*/
		if(map->current_msg!=map->latest_msg && map->msg_hist[map->current_msg+1]!=NULL){
			map->current_msg++;
			update_msg(map->msg_hist[map->current_msg], map->current_msg==map->latest_msg);
		}
		break;
	case SDLK_DELETE:
		if(map->current_msg!=0 && map->msg_hist[map->current_msg-1]!=NULL)
			update_msg(map->msg_hist[--map->current_msg], 0);
		break;
	case SDLK_RETURN:
		if(player->inventory != NULL && player->inventory[player->selected_item] != NULL) {
			spawn_uses_item (player, player->inventory[player->selected_item], map);
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
			spawn_action(map->spawns[i], map);
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
        props->has_target = true;
    }
    if (self->x == props->targetx && self->y == props->targety) {
        props->has_target = false;
    }

    if ((1 == abs(player->x - self->x) && 0 == abs(player->y - self->y)) ||
        (0 == abs(player->x - self->x) && 1 == abs(player->y - self->y))) {
        spawn_spawn_collision(self, player, map);
    } else if (props->has_target) {
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
            for (unsigned int ii = 0, jj = 0; ii < map->number_of_spawns; ++ii) {
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
            other->hp = 0;
            update_hp(self->hp);
        } else {
            other->hp -= 5;
            update_hp(self->hp);
        }
    }
}

void spawn_tile_collision (Spawn *self, Tile *tile, Map *map) {
	if(!tile_can_walk(*tile)) {
		/* umkehren */
		switch(self->direction) {
			case NORTH:
				self->y++;
				break;
			case SOUTH:
				self->y--;
				break;
			case EAST:
				self->x--;
				break;
			case WEST:
				self->x++;
				break;
		}
		if (tile->type == TILE_TYPE_WALL) {
			if (self->hp > 0) {
				--self->hp;
				update_hp(self->hp);
				push_msg("Kopf -> Wand (-1HP)", map);
			}
		}
	}
	else {
		if (tile->type == TILE_TYPE_FLOOR && tile->number_of_items > 0) {
			/* Items aufsammeln. */
			self->inventory = ex_realloc(self->inventory, (self->inventory_size + tile->number_of_items) * sizeof(Item*));
			for (unsigned int ii = 0; ii < tile->number_of_items; ++ii) {
				self->inventory[++self->inventory_size - 1] = tile->items[ii];
			}
			/* Aufzählen, was aufgesammelt */
			for(unsigned int i = 0; i < tile->number_of_items; ++i) {
				char* item_message = (char*)ex_calloc(22 + strlen(tile->items[i]->name), 1);				
				sprintf(item_message, "Du hast aufgenommen: %s", tile->items[i]->name);
				push_msg(item_message, map);
				free(item_message);
			}
			self->selected_item=self->inventory_size-1;
			update_item(self->inventory[self->selected_item]->name, self->selected_item+1);
			
			free(tile->items);
			tile->items = NULL;
			tile->number_of_items = 0;
		} 
		/* Ausgang */
		else if(tile->type == TILE_TYPE_EXIT) {
			map->finished = 1;
		}
	}
}

void spawn_uses_item (Spawn *self, Item *item, Map *map) {
	(void)map;
    switch (item->type) {
        case ITEM_TYPE_HEALTH_POTION:
            self->hp += ((HealthPotionProperties *)item->properties)->capacity;
            if (self->hp > self->max_hp) {
                self->hp = self->max_hp;
            }
			push_msg("Schluck! (Heiltrank)", map);
			update_hp(self->hp);
			/* raus mit dem Item ... */
			delete_item(item, self);
            break;
        default:
            break;
    }
}

/**
 * Löscht das erste Item vom Typ item->type aus dem Inventar des Spawns spawn.
 * Wenn spawn kein Item des Typs hat, bleibt das Inventar unverändert.
 * Gibt die Anzahl der gelöschten Items zurück (0 oder 1).
 */
static int delete_item(Item *item, Spawn *spawn){
	/*nur, wenn noch was im Inventar ist*/
	if(spawn->inventory_size>1){
		Item **result=(Item **)ex_calloc(spawn->inventory_size-1, sizeof(Item *));
		unsigned int ins=0;
		for(;spawn->inventory[ins]->type!=item->type;ins++){
			/*nicht im Inventar*/
			if(ins>=spawn->inventory_size){
				free(result);
				return 0;
			}
		}
		memcpy(result,spawn->inventory,ins*sizeof(Item *));
		/*zweiten Teil nur, wenn noch was über ist*/
		if(ins<spawn->inventory_size-1){
			memcpy(result+ins,spawn->inventory+ins+1,(spawn->inventory_size-ins-1)*sizeof(Item *));
		}
		free(spawn->inventory[ins]);
		free(spawn->inventory);
		spawn->inventory=result;
		spawn->inventory_size--;
		if(!spawn->npc){
			/*ausgewähltes Item anpassen*/
			spawn->selected_item=(spawn->selected_item>1) ? spawn->selected_item-1 : 0;
			/*Ausgabe anpassen*/
			update_item(spawn->inventory[spawn->selected_item]->name, spawn->selected_item+1);
		}
		return 1;
	}
	else{
		/*letztes Element wird gelöscht*/
		if(spawn->inventory_size==1){
			free(spawn->inventory[0]);
			free(spawn->inventory);
			spawn->inventory=NULL;
			spawn->inventory_size--;
			if(!spawn->npc){
				spawn->selected_item=0;
				update_item(NULL, -1);
			}
		}
		else
			return 0;
	}
	return 0;
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
	else if(self->type == TILE_TYPE_DOOR)  {
		DoorProperties* door_props = (DoorProperties *)self->properties;
		/* Tür hat eigenen Schalter und ist nicht verschlossen */
		if(!door_props->locked) {
			if(!(door_props->external_button)) {
				door_props->open ^= 1;
			}
		} else {
			/* Mit Schlüssel aufschließbar */
			if(door_props->key_id != NULL) {
				unsigned int i;
				Spawn* player = get_player_spawn(map);
				/* Inventar scannen */
				for(i = 0; i < player->inventory_size; ++i) {
					if(player->inventory[i] != NULL && player->inventory[i]->type == ITEM_TYPE_KEY) {
						if(strcmp(player->inventory[i]->id, door_props->key_id) == 0) {
							/* Key entfernen! */
							/*free_item(player->inventory[i]);
							player->inventory[i] = NULL;
							if(i == player->selected_item) {
								next_inventory_item(player, 1);
							}*/
							door_props->locked = 0;
							push_msg("Tuer entriegelt", map);
							break;
						}
					}
				}
				/* wurde nicht aufgemacht */
				if(door_props->locked) {
					push_msg("Tuer verschlossen", map);
				}
			}
		}
	}
	/* Hinweis */
	else if(self->type == TILE_TYPE_HINT)  {
		HintProperties* hint_props = (HintProperties *)self->properties;
		if(hint_props->message != NULL) {
			/* Was das Ding zu sagen hat, in den Ausgabestream packen! */
			push_msg(hint_props->message, map);
		}
	}
}
