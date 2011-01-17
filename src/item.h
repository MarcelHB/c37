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
	/* Name */
	char* name;
	/* 32-bit Farbe nach RGBA */
	unsigned long color;
	/* Gewicht, Wert */
	int weight;
	int value;
	/* Polymorphismusersatz für den eig. Typ */
	unsigned int type;
	/* spez. Eigenschaften des Items */
	void* properties;
 } Item;
 
 /* Spwan führt die Action eines Items aus (Trank trinken...) */
 void spawn_uses_item(Spawn*, Item*, Map*);
 
 /* Gibt reservierten Speicher eines Items frei */
 void free_item(Item*);
 
 /* Properties fuer Klunker/Waffe/Misc... */
 
 /* Zuordnungstabellen von type -> defaults */
 /* "healthpotion" -> "Health Potition", blau, 3lbs, 5GM ... */
 
 #endif