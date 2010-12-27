/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * item.h
 */

 #ifndef _C37_ITEM_H
 #define _C37_ITEM_H

 typedef struct Item {
	/* ID */
	char* id;
	/* 32-bit Farbe nach RGBA */
	unsigned long color;
	/* Gewicht, Wert */
	int weight;
	int value;
	/* Polymorphismusersatz f�r den eig. Typ */
	char* type;
	/* spez. Eigenschaften des Items */
	void* properties;
 } Item;
 
 /* Spwan f�hrt die Action eines Items aus (Trank trinken...) */
 void spawn_uses_item(Spawn*, Item*);
 
 /* Properties fuer Klunker/Waffe/Misc... */
 
 #endif