/**
 * C-Projekt von Gruppe 37
 * Autor: Marc
 * Datum: 17.01.2011
 * 
 * text_output.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "globals.h"

#ifdef __unix__ || __linux__
	#define CLRSCR() printf("\x1B[2J")
#elif __WIN32__ || _MSC_VER
	#define CLRSCR() system("cls")
#else
	/*dann bleibt das halt da stehen*/
	#define CLRSCR()
#endif

static int width=OUTPUT_IN_GLYPHS_X;
static int height=OUTPUT_IN_GLYPHS_Y;

/**
 * Setzt die Breite und Höhe des ausgegebenen Spielfeldes fest, sonst nur zur Kompatibilität
 * mit der sdl_output.
 * Produkt aus Breite und Höhe ist die Länge des Arrays, das an output_draw übergeben wird.
 */
void output_init(int wid, int hei){
	width=wid;
	height=hei;
	/*aufräumen*/
	CLRSCR();
}

/**
 * Gibt den Ausgabepuffer als einfachen Text auf der Konsole aus (Farben werden ignoriert).
 * Parameter:
 * buf - zu zeichnender Ausgabepuffer
 * tiles - Anzahl der Tiles darin (sollte mindestens width*height aus output_init() sein)
 */
void output_draw(BufferTile *buf, int tiles){
	int wid=width;
	while(tiles--){
		printf("%c",buf->glyph);
		buf++;
		/*zeilenumbruch nach width zeichen*/
		if(!(wid--)){
			printf("\n");
			wid=width;
		}
	}
}

/*
 * Löscht die Ausgabe
 */
void output_clear(){
	CLRSCR();
}

/*
 * Nur zur Kompatibilität mit sdl_output
 */
void output_close(){

}
