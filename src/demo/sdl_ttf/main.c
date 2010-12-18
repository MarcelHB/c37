#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_ttf.h"

#define LINES_X 80
#define LINES_Y 25

#define FONT_SIZE 12
#define FONT_RATIO 0.75
#define FONT_SPACE 1

const char* map = "_______\n|.....|\n|..@..|____\n|________ |\n        |i|\n        |#|\n        |#|\n";

int main (int argc, char* argv[]) {
	SDL_Surface* screen, *text;
	SDL_Event event;
	SDL_Color color = {255, 255, 255};
	TTF_Font* font;
	int quit = 0, i, map_size, buf_size = 0, current_line = 0;
	char* glyph_buffer = NULL;

	SDL_Init(SDL_INIT_VIDEO);
	if(TTF_Init() == -1) {
		fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}
	
	#ifdef __MINGW32__
	font = TTF_OpenFont("C:/Windows/Fonts/cour.ttf", FONT_SIZE);
	#else
	#error "Please rewrite code for unix ttf paths!"
	#endif
	if(!font) {
		fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
		exit(2);
	}
	
	screen = SDL_SetVideoMode(LINES_X * FONT_SIZE * FONT_RATIO, LINES_Y * FONT_SIZE + (LINES_X - 1) * FONT_SPACE, 16, SDL_HWSURFACE);
	SDL_WM_SetCaption("SDL_ttf Demo", NULL);
	
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	
	map_size = strlen(map);
	
	for(i = 0; i < map_size; ++i) {
		SDL_Rect position;
		switch (*(map+i)) {
			case '\n':
				if(buf_size == 0) {
					continue;
				}
				position.x = 0;
				position.y = -( (current_line * FONT_SPACE) + current_line * FONT_SIZE);
				position.w = LINES_X * FONT_SIZE * FONT_RATIO;
				position.h = FONT_SIZE + (LINES_X - 1) * FONT_SPACE;
				
				text = TTF_RenderText_Solid(font, glyph_buffer, color);
				SDL_BlitSurface(text, &position, screen, NULL);
				
				SDL_FreeSurface(text);
				text = NULL;
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
	
	TTF_CloseFont(font);
	SDL_Quit();
	return 0;
}