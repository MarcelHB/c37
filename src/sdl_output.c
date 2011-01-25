/**
 * C-Projekt von Gruppe 37
 * Autor: Marcel
 * Datum:
 * 
 * sdl_output.c
 */
 
 #include <stdlib.h>
 #include "SDL.h"
 #include "globals.h"
 #include "sdl_output.h"
 
 #define SDL_FONT_RECT_SIZE 16
 #define SDL_FONT_RATIO 0.75
 #define SDL_FONT_SPACE 1
 #define SDL_FONT_FILE "fonts/courier_new_12.bmp"
 
 static SDL_Surface* screen = NULL;
 static SDL_Surface* font = NULL;
 static int size_x = 0;
 static int size_y = 0;
 
 /*--------------------------------------------------------------------------*/
 void output_init(int x, int y) {
	SDL_Init(SDL_INIT_VIDEO); 
	screen = SDL_SetVideoMode(x * SDL_FONT_RECT_SIZE * SDL_FONT_RATIO, y * SDL_FONT_RECT_SIZE + (x - 1) * SDL_FONT_SPACE, 16, SDL_HWSURFACE);	
	size_x = x; size_y = y;
	
	SDL_WM_SetCaption("c37 (SDL)", NULL);
	font = SDL_LoadBMP(SDL_FONT_FILE);
	if(font == NULL) {
		fprintf(stderr, "SDL findet fonts/courier_new_12.bmp nicht!");
		exit(1);
	}
 }
 
 /*--------------------------------------------------------------------------*/
 void output_draw(BufferTile* buf, int num) {
	int i,j;
	(void)num;
	
	output_clear();
	
	for(i = 0; i < size_y; ++i) {
		for(j = 0; j < size_x; ++j) {
			SDL_Rect source_position;
			SDL_Rect dest_position;
			SDL_Surface* color_surface = NULL;
			SDL_Surface* stencil_surface = NULL;
			BufferTile* tile = &buf[i * size_x + j];
			
			source_position.x = (tile->glyph % SDL_FONT_RECT_SIZE) * SDL_FONT_RECT_SIZE + 4;
			source_position.y = (tile->glyph / SDL_FONT_RECT_SIZE) * SDL_FONT_RECT_SIZE + SDL_FONT_SPACE;
			source_position.h = SDL_FONT_RECT_SIZE - SDL_FONT_SPACE;
			source_position.w = SDL_FONT_RECT_SIZE / 2;
			dest_position.x = (SDL_FONT_RECT_SIZE-4)*j;
			dest_position.y = ( i  * SDL_FONT_RECT_SIZE);
			dest_position.w =  SDL_FONT_RECT_SIZE / 2;
			dest_position.h = SDL_FONT_RECT_SIZE;			
			
			/* per glyph color support */
			color_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, SDL_FONT_RECT_SIZE / 2, SDL_FONT_RECT_SIZE, 16, 0,0,0,0);
			stencil_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, SDL_FONT_RECT_SIZE / 2, SDL_FONT_RECT_SIZE, 16, 0,0,0,0);
			SDL_FillRect(color_surface, NULL, SDL_MapRGB(screen->format, tile->color >> 24, (tile->color >> 16) & 0xFF, (tile->color >> 8) & 0xFF));
			SDL_BlitSurface(font, &source_position, stencil_surface, NULL);
			SDL_SetColorKey(stencil_surface, SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 255, 255, 255));
			SDL_BlitSurface(stencil_surface, NULL, color_surface, NULL);
			SDL_BlitSurface(color_surface, NULL, screen, &dest_position);		
					
			SDL_FreeSurface(color_surface);
			SDL_FreeSurface(stencil_surface);
		}
	}
	SDL_UpdateRect(screen, 0, 0, 0, 0);
 }
 
 /*--------------------------------------------------------------------------*/
 void output_clear() {
	SDL_FillRect(screen, NULL, 0x00000000);
 }
 
 /*--------------------------------------------------------------------------*/
 void output_close() {
	SDL_FreeSurface(screen);
	SDL_FreeSurface(font);
	screen = NULL;
	font = NULL;
 }