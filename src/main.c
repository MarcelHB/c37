/**
 * C-Project von Gruppe 37
 *
 * main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"

#include "memory.h"
#include "globals.h"
#include "map.h"
#include "map_loader.h"
#include "output_buffer.h"
#include "sdl_output.h"
#include "action.h"
#include "main.h"
#include "spawn.h"

static Map *map;
static BufferTile *buf;

int main(int argc, char *argv[]){
	Spawn *player;
	InterfaceData idata = {0, NULL, -1, NULL, 1};
	SDL_Event event;
	int num_tiles = OUTPUT_IN_GLYPHS_X * OUTPUT_IN_GLYPHS_Y, i;
	
	/*SDL anmachen*/
    if(SDL_Init(SDL_INIT_VIDEO))
        return EXIT_FAILURE;
    SDL_EnableKeyRepeat(200, 50);
    
    /*Karte laden*/
    if(argc==2)
        map=load_map(argv[1]);
    else{
        fprintf(stderr,"Kartennamen angeben\n");
        return EXIT_FAILURE;
    }
    if(map == NULL) {
        fprintf(stderr,"Fehler beim Laden der Karte\n");
        return EXIT_FAILURE;
    }
    
    player = get_player_spawn(map);
    if(player==NULL){
        fprintf(stderr, "Kein Spieler auf der Karte\n");
        return EXIT_FAILURE;
    }
    
    /*Map zeichnen*/
    
    /* Ausgabepuffer initialisieren */
    buf = (BufferTile*)ex_malloc(sizeof(BufferTile) * num_tiles);
    for(i = 0; i < num_tiles; ++i) {
        BufferTile bt = {' ', 0x00000000};
        buf[i] = bt;
    }
    
    output_init(OUTPUT_IN_GLYPHS_X, OUTPUT_IN_GLYPHS_Y, map->name);
    
    explore_area(player, map);
    create_output_buffer(map, buf, num_tiles);
    get_interface_data(map, &idata);
    output_draw(buf, num_tiles, &idata);
    
    /*Eingabeloop*/
    int quit=0;
    KeyAction current_action = INVALID;
    while(SDL_WaitEvent(&event)){
        if(event.type == SDL_KEYDOWN) {
            current_action = get_action(event.key.keysym.sym);
            /*bei Escape beenden*/
            if(event.key.keysym.sym == SDLK_ESCAPE){
                quit=1;
                break;
            }
            if(current_action != INVALID) {
                process_event(current_action, map);
            }
            create_output_buffer(map, buf, num_tiles);
            get_interface_data(map, &idata);
            output_draw(buf, num_tiles, &idata);
        } else if(event.type == SDL_QUIT) {
            quit=1;
            break;
        }
        SDL_Delay(1);
        /*Affe tot => Klappe zu*/
        if(player->hp<=0){
            game_over(0);
            break;
        }
        /* Ende erreicht */
        if(map->finished) {
            game_over(1);
            break;
        }
    }
    if(!quit){
        while(SDL_WaitEvent(&event)){
            /*bei Escape beenden*/
            if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                break;
            }
            SDL_Delay(1);
        }
    }
    free(buf);
    flush_map(map);
    free(idata.message); free(idata.item_name);
    
    output_close();
    SDL_Quit();
    return EXIT_SUCCESS;
}

/*--------------------------------------------------------------------------*/
void create_output_buffer(Map* map, BufferTile* buf, int size) {
    /* Spieler in die Mitte, seine Position als Versatz benutzen */
    int i, j, translated_x, translated_y, center_x, center_y;
    Spawn* spawn = get_player_spawn(map);
    if(spawn == NULL) {
        fprintf(stderr, "Keine Spielfigur vorhanden!\n");
        exit(1);
    }
    clear_output_buffer(buf, size);

    center_x = OUTPUT_IN_GLYPHS_X / 2 + 1; center_y = OUTPUT_IN_GLYPHS_Y / 2 + 1;
    translated_x = center_x - spawn->x; translated_y = center_y - spawn->y;

    j = 0;
    for(i = 0; i < size; ++i) {
        unsigned int current_x, current_y;
        if(i != 0 && (i % (OUTPUT_IN_GLYPHS_X)) == 0) {
            ++j;
        }
        current_y = j;
        current_x = i % OUTPUT_IN_GLYPHS_X;
        /* unteres Renderende erreicht */
        if(current_y == OUTPUT_IN_GLYPHS_Y) {
            break;
        }
        /* Hier Kartenteil? */
        if(translated_x <= (int)current_x && translated_y <= (int)current_y && current_x < (translated_x + map->x) && current_y < (translated_y + map->y)) {
            render_tile(&buf[i], &map->tiles[(current_y - translated_y) * map->x + (current_x - translated_x)], map);
        }
    }
}

 /*--------------------------------------------------------------------------*/
 void clear_output_buffer(BufferTile* buf, int num) {
    int i = 0;
    for(; i < num; ++i) {
        buf[i].glyph = ' ';
        buf[i].color = 0x00000000;
    }
 }
 
 /*--------------------------------------------------------------------------*/
 void get_interface_data(Map* map, InterfaceData* id) {
    Spawn* player = get_player_spawn(map);
    
    /* Lebenspunkte Spieler */
    id->player_hp = player->hp;
    
    free(id->item_name); free(id->message);
    
    if(player->inventory != NULL && player->inventory[player->selected_item] != NULL && player->inventory[player->selected_item]->type != ITEM_TYPE_INVALID) {
        id->item_name = (char*)ex_calloc(strlen(player->inventory[player->selected_item]->name) + 1, 1);
        strcpy(id->item_name, player->inventory[player->selected_item]->name);
        id->item_index = player->selected_item + 1;
    } else {
        /* keines vorhanden */
        id->item_index = -1;
        id->item_name = NULL;
    }
    
    /* aktuelle Nachricht */
    if(map->msg_hist[map->current_msg] != NULL) {
        id->message = (char*)ex_calloc(strlen(map->msg_hist[map->current_msg]) + 1, 1);
        strcpy(id->message, map->msg_hist[map->current_msg]);
    }
    id->last_message = map->current_msg == map->latest_msg ? 1 : 0;
 }

 /*--------------------------------------------------------------------------*/
 KeyAction get_action(SDLKey key) {
    if(key == SDLK_UP) {    
        return UP;
    }
    else if(key == SDLK_DOWN) {    
        return DOWN;
    }
    else if(key == SDLK_LEFT) {    
        return LEFT;
    }
    else if(key == SDLK_RIGHT) {    
        return RIGHT;
    }
    else if(key == SDLK_SPACE) {    
        return ACTION;
    }
    else if(key == SDLK_PAGEUP) {    
        return NEXT_ITEM;
    }
    else if(key == SDLK_PAGEDOWN) {    
        return PREV_ITEM;
    }
    else if(key == SDLK_INSERT) {    
        return NEXT_MSG;
    }
    else if(key == SDLK_DELETE) {    
        return PREV_MSG;
    }
    else if(key == SDLK_RETURN) {    
        return USE;
    }
    else if(key == SDLK_PERIOD) {    
        return REST;
    }
    return INVALID;
 }
