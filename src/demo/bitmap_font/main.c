#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "SDL.h"

#define LINES_X 80
#define LINES_Y 25

#define FONT_FILE "courier_new_12.bmp"
#define FONT_RECT_SIZE 16

#define FONT_SIZE 12
#define FONT_RATIO 0.75
#define FONT_SPACE 1

const char* map = "_______\n|.....|\n|..@..|____\n|________ |\n        |i|\n        |#|\n        |#|\n";

int main (int argc, char* argv[]) {
	SDL_Surface* screen, *text;
	SDL_Event event;
	SDL_Color color_white = {255, 255, 255};
	SDL_Color color_gray = {200, 200, 200};
	int quit = 0, i, map_size, buf_size = 0, current_line = 0;
	char* glyph_buffer = NULL;

	SDL_Init(SDL_INIT_VIDEO);
		
	screen = SDL_SetVideoMode(LINES_X * FONT_SIZE * FONT_RATIO, LINES_Y * FONT_SIZE + (LINES_X - 1) * FONT_SPACE, 16, SDL_HWSURFACE);
	SDL_WM_SetCaption("SDL bitmap fonts Demo", NULL);
	
	text = SDL_LoadBMP(FONT_FILE);
	
	if(text == NULL) {
		fprintf(stderr, "Couldn't load %s!", FONT_FILE);
		exit(1);
	}
	
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	
	map_size = strlen(map);
	
	for(i = 0; i < map_size; ++i) {
		int j;
		
		switch (*(map+i)) {
			case '\n':				
				if(buf_size == 0) {
					continue;
				}
				
				for(j = 0; j < buf_size; ++j) {
					SDL_Rect source_position;
					SDL_Rect dest_position;
					int current_char = glyph_buffer[j];
					
					// per glyph color support
					SDL_Surface* color_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, FONT_RECT_SIZE / 2, FONT_RECT_SIZE, 16, 0,0,0,0);
					SDL_Surface* stencil_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, FONT_RECT_SIZE / 2, FONT_RECT_SIZE, 16, 0,0,0,0);
					SDL_FillRect(color_surface, NULL, SDL_MapRGB(screen->format, rand()%256, rand()%256, rand()%256));					
					// make glyphs a bit compacter
					source_position.x = (current_char % FONT_RECT_SIZE) * FONT_RECT_SIZE + 4;
					source_position.y = (current_char / FONT_RECT_SIZE) * FONT_RECT_SIZE + 1;
					source_position.h = FONT_RECT_SIZE;
					source_position.w = FONT_RECT_SIZE / 2;
					
					dest_position.x = (FONT_RECT_SIZE-4)*j;
					dest_position.y = ( current_line  * FONT_RECT_SIZE);
					dest_position.w =  FONT_RECT_SIZE / 2;
					dest_position.h = FONT_RECT_SIZE;
					
					SDL_BlitSurface(text, &source_position, stencil_surface, NULL);
					SDL_SetColorKey(stencil_surface, SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 255, 255, 255));
					SDL_BlitSurface(stencil_surface, NULL, color_surface, NULL);
					SDL_BlitSurface(color_surface, NULL, screen, &dest_position);		
					
					SDL_FreeSurface(color_surface);
					SDL_FreeSurface(stencil_surface);
				}
				free(glyph_buffer);
				glyph_buffer = NULL;
				
				buf_size = 0;
				++current_line;
				break;
			default:
				glyph_buffer = realloc(glyph_buffer, sizeof(char) * (++buf_size));
				*(glyph_buffer + buf_size - 1) = *(map+i);
				break;
		}
	}
	
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	
	while (SDL_WaitEvent(&event) && !quit){
		if(event.type == SDL_KEYDOWN){
			switch (event.key.keysym.sym){
				case SDLK_ESCAPE:
					quit = 1;
					break;
			}
		}
	}
	SDL_FreeSurface(text);
	text = NULL;
	free(glyph_buffer);
	
	SDL_Quit();
	return 0;
}