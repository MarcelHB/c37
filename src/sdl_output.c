/**
 * C-Projekt von Gruppe 37
 * 
 * sdl_output.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include "globals.h"
#include "output_buffer.h"
#include "memory.h"
#include "sdl_output.h"

#define SDL_FONT_RECT_SIZE 16
#define SDL_FONT_RATIO 0.75
#define SDL_FONT_SPACE 1
#define SDL_FONT_FILE "fonts/courier_new_12.bmp"

/*maximale Anzahl an Zeilen, die für Nachrichten überschrieben werden, danach abschneiden*/
#define MAX_MSG_LINES 3

static int width = 0;
static int height = 0;
static int font_w = SDL_FONT_RECT_SIZE * SDL_FONT_RATIO;
static int font_h = SDL_FONT_RECT_SIZE;
static SDL_Surface *screen, *icon, *font, *color_surface, *stencil_surface;

/* private */
void output_draw_text(char*, SDL_Rect*, unsigned long);
void output_draw_glyph(unsigned char, SDL_Rect*, unsigned long);

/**
 * Macht die Ausgabe bereit zum Schreiben von Spielfeldern mit einer Breite von w
 * und einer Höhe von h Zeichen und dem Title c37: mapname.
 */
void output_init(int w, int h, char *mapname){
    /* SDL anmachen, falls es noch nicht ist */
    if(!SDL_WasInit(SDL_INIT_VIDEO)) {
        if(SDL_Init(SDL_INIT_VIDEO)) {
            fprintf(stderr, "SDL geht nicht an: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }

    /* Schrift laden */
    font = SDL_LoadBMP(SDL_FONT_FILE);
    if(font == NULL) {
        fprintf(stderr, "SDL findet fonts/courier_new_12.bmp nicht!");
        exit(1);
    }
    
    color_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, SDL_FONT_RECT_SIZE / 2, SDL_FONT_RECT_SIZE, 16, 0,0,0,0);
		stencil_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, SDL_FONT_RECT_SIZE / 2, SDL_FONT_RECT_SIZE, 16, 0,0,0,0);
    
    /* Screen erstellen, einen höher für Statusleiste */
    icon = SDL_LoadBMP("icon.ico");
    SDL_WM_SetIcon(icon,NULL);

    if(mapname != NULL) {
        char *title=(char *)ex_calloc(6+strlen(mapname), sizeof(char));
        sprintf(title, "c37: %s", mapname);
        SDL_WM_SetCaption(title, title);
        free(title);
    } else {
        SDL_WM_SetCaption("c37", "c37");
    }
   
    screen = SDL_SetVideoMode(OUTPUT_IN_GLYPHS_X*font_w, (OUTPUT_IN_GLYPHS_Y+1)*font_h, 16, SDL_HWSURFACE);
    if(screen == NULL){
        fprintf(stderr, "Kann keinen Output erstellen: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_UpdateRect(screen, 0, 0, 0, 0);
    width = w;
    height = h;
}

/* gibt den Ausgabepuffer mittels SDL aus */
void output_draw(BufferTile *buf, int tiles, InterfaceData* id){
    output_clear();
    
    /* wenns eine Nachricht gibt, dafür Platz machen */    
    int msg_lines;
    if(id->message != NULL){
        msg_lines = ((strlen(id->message)-1)/width)+1;
        if(msg_lines>MAX_MSG_LINES) {
            msg_lines = MAX_MSG_LINES;
        }
        if(tiles>(height-msg_lines)*width) {
            tiles = (height-msg_lines)*width;
        }
    }
    
    int num = 0;
    while(tiles--) {
        SDL_Rect pos;
        pos.x = (num%width)*font_w;
        pos.y = (num/width)*font_h;
        
        output_draw_glyph(buf->glyph, &pos, buf->color);
        buf++;
        num++;
    }
    
    SDL_Color white;
    white.r = white.g = white.b = 255;
    
    /* Nachricht */
    if(id->message != NULL && strlen(id->message) != 0) {
        char *msg_tmp = id->message;
        
        while(msg_lines--) {
            char *tmp = (char*)ex_calloc(width+1, sizeof(char));
            strncpy(tmp, id->message, width);
            tmp[width] = '\0';
            id->message += width;
                       
            SDL_Rect pos;
            pos.x = 0;
            pos.y = (height-msg_lines-1)*font_h;
            
            /* aktuelle Nachricht orange machen */
            if(id->last_message) {
                output_draw_text(tmp, &pos, 0xFFC80000);
            } else {
                output_draw_text(tmp, &pos, 0xFFFFFF00);
            }
            free(tmp);
        }
        id->message = msg_tmp;
    }
    
    char *status;
    /* keine Zahl vorm Item, wenn <0 */
    if(id->item_index < 0) {
        status = (char*)ex_calloc(width+1, sizeof(char));
        sprintf(status, "HP: %03d | Item: ", id->player_hp);
    } else {
        int intlen = 0, tmp = id->item_index;
        while(tmp /= 10) {
            intlen++;
        }
        status = (char*)ex_calloc(width+intlen+3, sizeof(char));
        sprintf(status, "HP: %03d | Item %d: ", id->player_hp, id->item_index);

    }
    
    if(id->item_name == NULL) {
        strcat(status, "--");
    } else {
        strncat(status, id->item_name, width-16);
    }
    
    /* ganz unten anfügen */
    SDL_Rect pos;
    pos.x = 0;
    pos.y = height*font_h;
    output_draw_text(status, &pos, 0xFFFFFF00);

    free(status);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/**
 * Großes rotes GAME OVER oder grünes WELL DONE
 */
void game_over(int successful) {    
    /* Ausgabe */
    if(!successful) {
        SDL_Rect pos;
        pos.x = (screen->w-(font_w*9))/2;
        pos.y = (screen->h-font_h)/2;
        output_draw_text("GAME OVER",&pos, 0xFF000000);
    } else {
        SDL_Rect pos;
        pos.x = (screen->w-(font_w*12))/2;
        pos.y = (screen->h-font_h)/2;
        output_draw_text("WELL DONE :)", &pos, 0x00FF0000);
    }
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}



void output_draw_text(char* text, SDL_Rect* pos, unsigned long color) {
    while(*text != 0) {
        pos->x += font_w;
        output_draw_glyph(*text, pos, color);
        
        ++text;
    }
}

void output_draw_glyph(unsigned char glyph, SDL_Rect* pos, unsigned long color) {
    SDL_Rect source_position, dest_position;
    
    source_position.x = (glyph % SDL_FONT_RECT_SIZE) * SDL_FONT_RECT_SIZE + 4;
    source_position.y = (glyph / SDL_FONT_RECT_SIZE) * SDL_FONT_RECT_SIZE + SDL_FONT_SPACE;
    source_position.h = SDL_FONT_RECT_SIZE - SDL_FONT_SPACE;
    source_position.w = SDL_FONT_RECT_SIZE / 2;
    dest_position.x = pos->x;
    dest_position.y = pos->y;
    dest_position.w =  SDL_FONT_RECT_SIZE / 2;
    dest_position.h = SDL_FONT_RECT_SIZE;			

    /* per glyph color support */
    SDL_FillRect(color_surface, NULL, SDL_MapRGB(screen->format, color >> 24, (color >> 16) & 0xFF, (color >> 8) & 0xFF));
    SDL_BlitSurface(font, &source_position, stencil_surface, NULL);
    SDL_SetColorKey(stencil_surface, SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 255, 255, 255));
    SDL_BlitSurface(stencil_surface, NULL, color_surface, NULL);
    SDL_BlitSurface(color_surface, NULL, screen, &dest_position);
}


/* löscht die Ausgabe */
void output_clear(){
    SDL_FillRect(screen, NULL, 0);
}
/* Ausgabedevices schließen */
void output_close(){
    SDL_FreeSurface(font);
    SDL_FreeSurface(screen);
    SDL_FreeSurface(icon);
    SDL_FreeSurface(color_surface);
    SDL_FreeSurface(stencil_surface);
}
