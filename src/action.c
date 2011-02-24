/**
 * C-Projekt von Gruppe 37
 * 
 * action.c
 */

#include <stdbool.h>
#include <stdlib.h>
#include "globals.h"
#include "memory.h"
#include "map.h"
#include "spawn.h"
#include "tile.h"
#include "action.h"
#include "sdl_output.h"

/*Helper, löscht Item*/
void delete_item(Item*, Spawn*);
int delete_item_by_type(unsigned int, Spawn*);
void damage_spawn(Spawn*, unsigned int, Map*);

/*aktualisiert eine Map gemäß einem Event*/
void process_event(KeyAction action, Map *map){
    Spawn *player=get_player_spawn(map);
    Spawn *npc;
    Tile *tile;
    bool should_run_ai = true;
    /*Aktion des Spielers*/
    switch(action){
    case UP:
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
    case DOWN:
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
    case LEFT:
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
    case RIGHT:
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
    case ACTION:
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
    case NEXT_ITEM:
        should_run_ai = false;
        /*Inventar durchschalten*/
        if(player->inventory!=NULL && player->inventory_size!=0){
            player->selected_item=(player->selected_item+1)%player->inventory_size;
        }
        break;
    case PREV_ITEM:
        should_run_ai = false;
        if(player->inventory!=NULL && player->inventory_size!=0){
            if(player->selected_item)
                player->selected_item=player->selected_item-1;
            else
                player->selected_item=player->inventory_size-1;
        }
        break;
    case NEXT_MSG:
        should_run_ai = false;
        /*durch die History scrollen, nicht wrappen*/
        if(map->current_msg!=map->latest_msg && map->msg_hist[map->current_msg+1]!=NULL){
            map->current_msg++;
        }
        break;
    case PREV_MSG:
        should_run_ai = false;
        if(map->current_msg!=0 && map->msg_hist[map->current_msg-1]!=NULL)
            --map->current_msg;
        break;
    case USE:
        if(player->inventory != NULL && player->inventory[player->selected_item] != NULL) {
            spawn_uses_item (player, player->inventory[player->selected_item], map);
        }
        break;
    default:
        break;
    }
    /*jetzt handeln die NPCs*/
    unsigned int i;
    if (should_run_ai) {
        for(i=0;i<map->number_of_spawns;i++){
            /*den Spieler auslassen*/
            if(map->spawns[i]->npc)
                spawn_action(map->spawns[i], map);
        }
    }
}

void spawn_action(Spawn *spawn, Map *map){
    /*soll hier nach typ geswitcht werden?
     * spawn_run_ai geht atm ja nur für hunde*/
    switch(spawn->type){
    case SPAWN_TYPE_HOUND:
        spawn_run_ai(spawn, map);
        break;
    case SPAWN_TYPE_ZOMBIE:
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
        if (!tile_can_walk(map->tiles[ny * map->x + nx]) || NULL != other) {
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
        if (!tile_can_walk(map->tiles[ny * map->x + nx]) || NULL != other) {
            nx = self->x;
            ny = self->y;
        }
    }
    self->x = nx;
    self->y = ny;
}

/**
 * Fügt einem Spawn damage Punkte Schaden zu, entfernt ihn von der Map
 * und lässt seine Items fallen, falls es ihn tötet.
 */
void damage_spawn(Spawn *spawn, unsigned int damage, Map *map){
    /*Sonderbahandlung für den Spieler*/
    if(spawn->type==SPAWN_TYPE_PLAYER){
        if(spawn->hp<=damage)
            spawn->hp=0;
        else
            spawn->hp-=damage;
    }
    else{
        /*wenns ihn tötet, entfernen*/
        if (spawn->hp <= damage) {
            Spawn **new_spawns = ex_calloc(map->number_of_spawns - 1, sizeof(Spawn *));
            for (unsigned int ii = 0, jj = 0; ii < map->number_of_spawns; ++ii) {
                if (map->spawns[ii] != spawn) {
                    new_spawns[jj++] = map->spawns[ii];
                }
            }
            free(map->spawns);
            map->spawns = new_spawns;
            --map->number_of_spawns;
    
            /* Items fallen lassen */
            if(spawn->inventory_size > 0) {
                unsigned int i, j, new_size;
                Tile* died_here = &map->tiles[spawn->y * map->x + spawn->x];
                new_size = died_here->number_of_items + spawn->inventory_size;
                died_here->items = (Item**)ex_realloc(died_here->items, new_size * sizeof(Item*));
    
                for(i = died_here->number_of_items, j = 0; i < new_size && j < spawn->inventory_size; ++i, ++j, ++died_here->number_of_items, --spawn->inventory_size) {
                    died_here->items[i] = spawn->inventory[j];
                }
    
                free(spawn->inventory);
                spawn->inventory = NULL;
            }
            free_spawn(spawn);
        }
        else {
            spawn->hp -= damage;
        }
    }
}

void spawn_spawn_collision (Spawn *self, Spawn *other, Map *map) {
    switch(self->type){
    case SPAWN_TYPE_PLAYER:
        switch(other->type){
        case SPAWN_TYPE_HOUND:
            damage_spawn(other, 10, map);
            break;
        case SPAWN_TYPE_ZOMBIE:
            damage_spawn(other, 10, map);
            break;
        default:
            break;
        }
        break;
    case SPAWN_TYPE_HOUND:
        switch(other->type){
        case SPAWN_TYPE_PLAYER:
            damage_spawn(other, 5, map);
            push_msg("gebissen! (-5HP)", map);
            break;
        default:
            break;
        }
        break;
    case SPAWN_TYPE_ZOMBIE:
        switch(other->type){
        case SPAWN_TYPE_PLAYER:
            damage_spawn(other, 10, map);
            push_msg("Baem! (-10HP)", map);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void spawn_tile_collision (Spawn *self, Tile *tile, Map *map) {
    /* Kartenrand / unbegehbar */
    if(self->x >= map->x || self->y >= map->y || !tile_can_walk(*tile)) {
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
            damage_spawn(self, 1, map);
            push_msg("Kopf -> Wand (-1HP)", map);
        }
    }
    else {
        if (tile->number_of_items > 0) {
            /* Items aufsammeln. */
            self->inventory = ex_realloc(self->inventory, (self->inventory_size + tile->number_of_items) * sizeof(Item*));
            for (unsigned int ii = 0; ii < tile->number_of_items; ++ii) {
                if(tile->items[ii]->type != ITEM_TYPE_INVALID) {
                    self->inventory[++self->inventory_size - 1] = tile->items[ii];
                }
            }
            /* Aufzählen, was aufgesammelt */
            for(unsigned int i = 0; i < tile->number_of_items; ++i) {
                if(tile->items[i]->type != ITEM_TYPE_INVALID) {
                    char* item_message = (char*)ex_calloc(22 + strlen(tile->items[i]->name), 1);                
                    sprintf(item_message, "Du hast aufgenommen: %s", tile->items[i]->name);
                    push_msg(item_message, map);
                    free(item_message);
                }
            }
            self->selected_item=self->inventory_size-1;
            
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
        /* raus mit dem Item ... */
        delete_item(item, self);
        break;
    case ITEM_TYPE_DEAD_CAT:
        push_msg("Miau?", map);
        break;
	case ITEM_TYPE_SHOTGUN:
	    /*Schaden anrichten, wenn noch Muni da war*/
	    if(delete_item_by_type(ITEM_TYPE_SHOTGUN_AMMO, self)){
	        Spawn *victim=NULL;
            unsigned int damage=0;
            switch(self->direction){
            case NORTH:
		        for(unsigned int i=1;i<=10 && self->y-i+1;i++){
                    /*wenn da ne Wand ist, nichts tun*/
                    if(!tile_can_walk(map->tiles[map->x*(self->y-i)+self->x]))
                        break;
		            victim=get_spawn_at(self->x, self->y-i, map);
                    if(victim!=NULL){
                        /*weniger bei mehr Entfernung*/
                        damage=(11-i)*3;
                        break;
                    }
                }
	        	break;
             case SOUTH:
		        for(unsigned int i=1;i<=10 && self->y+i<map->y;i++){
                    /*wenn da ne Wand ist, nichts tun*/
                    if(!tile_can_walk(map->tiles[map->x*(self->y+i)+self->x]))
                        break;
		            victim=get_spawn_at(self->x, self->y+i, map);
                    if(victim!=NULL){
                        damage=(11-i)*3;
                        break;
                    }
                }
	        	break;
             case WEST:
		        for(unsigned int i=1;i<=10 && self->x-i+1;i++){
                    /*wenn da ne Wand ist, nichts tun*/
                    if(!tile_can_walk(map->tiles[map->x*self->y+self->x-i]))
                        break;
		            victim=get_spawn_at(self->x-i, self->y, map);
                    if(victim!=NULL){
                        damage=(11-i)*3;
                        break;
                    }
                }
	        	break;
             case EAST:
		        for(unsigned int i=1;i<=10 && self->x+i<map->x;i++){
                    /*wenn da ne Wand ist, nichts tun*/
                    if(!tile_can_walk(map->tiles[map->x*self->y+self->x+i]))
                        break;
		            victim=get_spawn_at(self->x+i, self->y, map);
                    if(victim!=NULL){
                        damage=(11-i)*3;
                        break;
                    }
                }
	        	break;
         default:
                break;
            }
            if(victim!=NULL)
                damage_spawn(victim, damage, map);
        }
        else
            push_msg("Keine Munition mehr!", map);
	    break;
    default:
        break;
    }
}

/*---------------------------------------------------------------------------*/
void delete_item(Item *item, Spawn *spawn) {
    /* wenn noch mehr im Inventar ist */
    if(spawn->inventory_size > 1) {
        unsigned int new_size = spawn->inventory_size - 1, i, j;
        Item** new_inventory = (Item**)ex_malloc(new_size * sizeof(Item*));
        
        for(i = 0, j = 0; i < spawn->inventory_size; ++i) {
            if(spawn->inventory[i] != item) {
                new_inventory[j] = spawn->inventory[i];
                ++j;
            } else {
                free_item(spawn->inventory[i]);
            }
        }
        
        --spawn->inventory_size;
        free(spawn->inventory);
        spawn->inventory = new_inventory;
        
        if(!spawn->npc) {
            /* ausgewähltes Item anpassen */
            spawn->selected_item = (spawn->selected_item > 1) ? spawn->selected_item - 1 : 0;
        }
    } 
    /* nur letztes Element wird gelöscht */
    else if(spawn->inventory_size == 1) {
        free_item(spawn->inventory[0]);
        free(spawn->inventory);
        spawn->inventory = NULL;
        --spawn->inventory_size;
        
        if(!spawn->npc) {
            spawn->selected_item = 0;
        }
    }
}

/* Löscht das erste Item vom Typ type aus dem Inventar von Spawn
 * Returnt 1, wenn was gelöscht wurde, 0 sonst.
 */
int delete_item_by_type(unsigned int type, Spawn *spawn){
    /* wenn noch mehr im Inventar ist */
    if(spawn->inventory_size > 1) {
        unsigned int new_size = spawn->inventory_size - 1, i, j;
        bool update_selected=false, deleted=false;
        Item** new_inventory = (Item**)ex_malloc(new_size * sizeof(Item*));
        
        for(i = 0, j = 0; i < spawn->inventory_size; ++i) {
            if(spawn->inventory[i]->type != type || deleted) {
                new_inventory[j] = spawn->inventory[i];
                ++j;
            } else {
                free_item(spawn->inventory[i]);
                deleted=true;
                if(i<=spawn->selected_item)
                    update_selected=true;
            }
        }
        free(spawn->inventory);
        spawn->inventory = new_inventory;
        if(deleted){
            --spawn->inventory_size;
            
            if(!spawn->npc && update_selected) {
                /* ausgewähltes Item anpassen */
                spawn->selected_item = (spawn->selected_item > 1) ? spawn->selected_item - 1 : 0;
            }
        	return 1;
        }
        else
            return 0;
    } 
    /* nur letztes Element wird gelöscht */
    else if(spawn->inventory_size == 1 && spawn->inventory[0]->type==type) {
        free_item(spawn->inventory[0]);
        free(spawn->inventory);
        spawn->inventory = NULL;
        --spawn->inventory_size;
        
        if(!spawn->npc) {
            spawn->selected_item = 0;
        }
        return 1;
    }
    else
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
                }
                /* Wand */
                else if(to_toggle->type == TILE_TYPE_WALL)  {
                    WallProperties* wall_props = (WallProperties *)to_toggle->properties;
                    wall_props->floor ^= 1;
                }
                else {
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
                    Item* key_candidate = player->inventory[i];
                    if(key_candidate != NULL && key_candidate->type == ITEM_TYPE_KEY) {
                        if(strcmp(key_candidate->id, door_props->key_id) == 0) {
                            /* Key entfernen! */
                            delete_item(key_candidate, player);
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
