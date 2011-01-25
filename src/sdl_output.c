/**
 * C-Projekt von Gruppe 37
 * 
 * sdl_output.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include "SDL_ttf.h"
#include "globals.h"
#include "output_buffer.h"
#include "memory.h"

#define FONT "fonts/cour.ttf"
#define FONT_SIZE 12
/*maximale Anzahl an Zeilen, die für Nachrichten überschrieben werden, danach abschneiden*/
#define MAX_MSG_LINES 3

static int width=0;
static int height=0;
static TTF_Font *font;
static int font_w=FONT_SIZE;
static int font_h=FONT_SIZE;
static SDL_Surface *screen;

static unsigned int hp=100;
static char *inv=NULL;
static char *msg=NULL;

/**
 * Macht die Ausgabe bereit zum Schreiben von Spielfeldern mit einer Breite von w
 * und einer Höhe von h Zeichen.
 */
void output_init(int w, int h){
	/*SDL anmachen, falls es noch nicht ist*/
	if(!SDL_WasInit(SDL_INIT_VIDEO)){
		if(SDL_Init(SDL_INIT_VIDEO)){
			fprintf(stderr, "SDL geht nicht an: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}
	}
	TTF_Init();
	/*Schrift laden*/
	font=TTF_OpenFont(FONT, FONT_SIZE);
	if(font==NULL){
		fprintf(stderr, "Schrift kann nicht geladen werden: %s\n", TTF_GetError());
		exit(EXIT_FAILURE);
	}
	/*sollte für alle Zeichen gelten (Festbreitenschrift)*/
	TTF_SizeText(font, "a", &font_w, &font_h);
	/*Screen erstellen, einen höher für Statusleiste*/
	screen=SDL_SetVideoMode(OUTPUT_IN_GLYPHS_X*font_w, (OUTPUT_IN_GLYPHS_Y+1)*font_h, 16, SDL_HWSURFACE);
	if(screen==NULL){
		fprintf(stderr, "Kann keinen Output erstellen: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	width=w;
	height=h;
}

/* gibt den Ausgabepuffer mittels SDL aus */
void output_draw(BufferTile *buf, int tiles){
	output_clear();
	/*wenns eine Nachricht gibt, dafür Platz machen*/	
	int msg_lines;
	if(msg!=NULL){
		msg_lines=((strlen(msg)-1)/width)+1;
		if(msg_lines>MAX_MSG_LINES)
			msg_lines=MAX_MSG_LINES;
		if(tiles>(height-msg_lines)*width)
			tiles=(height-msg_lines)*width;
	}
	int num=0;
	while(tiles--){
		/*Farbe*/
		SDL_Color color;
		color.r = buf->color >> 24;
		color.g = (buf->color >> 16) & 0xFF;
		color.b = (buf->color >> 8) & 0xFF;
		/*Text-Surface*/
		char glyph_text[2];
		glyph_text[0]=buf->glyph;
		glyph_text[1]='\0';
		SDL_Surface *glyph_surf=TTF_RenderText_Solid(font, glyph_text, color);
		if(glyph_surf==NULL){
			fprintf(stderr, "Fehler beim Darstellen von '%c': %s\n", buf->glyph, TTF_GetError());
			exit(EXIT_FAILURE);
		}
		/*auf den Screen bringen*/
		SDL_Rect pos;
		/*pos.w, pos.h werden nicht beachtet*/
		pos.x=(num%width)*font_w;
		pos.y=(num/width)*font_h;
		if(SDL_BlitSurface(glyph_surf, NULL, screen, &pos)){
			fprintf(stderr, "Fehler bei der Ausgabe: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}
		SDL_FreeSurface(glyph_surf);
		glyph_surf=NULL;
		buf++;
		num++;
	}
	SDL_Color white;
	white.r=white.g=white.b=255;
	/*Nachricht*/
	if(msg!=NULL){
		while(msg_lines--){
			char *tmp=(char *)ex_calloc(width+1, sizeof(char));
			strncpy(tmp, msg, width);
			tmp[width]='\0';
			msg+=width;
			SDL_Surface *msg_surf=TTF_RenderText_Solid(font, tmp, white);
			free(tmp);
			if(msg_surf==NULL){
				fprintf(stderr, "Fehler beim Darstellen einer Nachricht: %s\n", TTF_GetError());
				exit(EXIT_FAILURE);
			}
			SDL_Rect pos;
			pos.x=0;
			pos.y=(height-msg_lines)*font_h;
			if(SDL_BlitSurface(msg_surf, NULL, screen, &pos)){
				fprintf(stderr, "Fehler bei der Ausgabe einer Nachricht: %s\n", SDL_GetError());
				exit(EXIT_FAILURE);
			}
			free(msg_surf);
			msg_surf=NULL;
		}
		msg=NULL;
	}
	/*Statusleiste, width sollte mindestens 18 sein*/
	if(width<18){
		fprintf(stderr, "Das Spielfeld muss mindestens 18 Zeichen breit sein, sonst ist kein Platz fuer die Statusleiste.");
		exit(EXIT_FAILURE);
	}
	char *status=(char *)ex_calloc(width+1, sizeof(char));
	sprintf(status, "HP: %03d | Item: ", hp);
	if(inv==NULL)
		strcat(status, "--");
	else
		strncat(status, inv, width-16);
	SDL_Surface *status_surf=TTF_RenderText_Solid(font, status, white);
	free(status);
	if(status_surf==NULL){
		fprintf(stderr,"Fehler beim Darstellen der Statusleiste: %s\n", TTF_GetError());
		exit(EXIT_FAILURE);
	}
	/*ganz unten anfügen*/
	SDL_Rect pos;
	pos.x=0;
	pos.y=height*font_h;
	if(SDL_BlitSurface(status_surf, NULL, screen, &pos)){
		fprintf(stderr, "Fehler bei der Ausgabe der Stausleiste: %s\n",SDL_GetError());
		exit(EXIT_FAILURE);
	}
	free(status_surf);
	status_surf=NULL;
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/**
 * Aktualisiert die Statusleiste, sodass sie beim nächsten Neuzeichnen
 * des Bildschirms h Lebenspunkte anzeigt.
 */
void update_hp(int h){
	hp=h;
}

/**
 * Aktualisiert die Statusleiste, sodass sie beim nächsten Neuzeichnen
 * des Bildschirms item als ausgewähltes Item anzeigt.
 * Wenn item NULL ist, steht an der Stelle "--".
 */
void update_item(char *item){
	inv=(char *)ex_realloc(inv, sizeof(char)*(strlen(item)+1));
	strcpy(inv,item);
}

/**
 * Beim der nächsten Aktualisierung des Bildschirms erscheint die Nachricht m
 * über der Statusleiste (und überschreibt dabei wahrscheinlich einen Teil des Spielfelds).
 * Die Nachricht verschwindet bei der darauffolgenden Aktualisierung wieder.
 * Muss vor output_draw aufgerufen werden, da diese dir Nachricht auf NULL setzt.
 */
void print_msg(char *m){
	msg=m;
}

/* löscht die Ausgabe */
void output_clear(){
	SDL_FillRect(screen, NULL, 0);
}

/* Ausgabedevices schließen */
void output_close(){
	TTF_CloseFont(font);
	/*SDL wird in der main zugemacht*/
}
