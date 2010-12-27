/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * spawn.h
 */

 #ifndef _C37_SPAWN_H
 #define _C37_SPAWN_H

 typedef struct Spawn {
	/* ID */
	char* id;
	/* lesbarer Name */
	char* name;
	/* Platzierung */
	unsigned int x,y;
	/* Blickrichtung */
	char direction;
	/* ASCII-Zeichen zur Anzeige */
	char glyph;
	/* NPC? */
	char npc;
	/* humanoid? */
	char humanoid;
	/* (max) HPs */;
	unsigned int max_hp, hp;
	/* Polymorphismusersatz für den eig. Typ */
	char* type;
	/* spez. Eigenschaften des Spawns */
	void* properties;
	/* Besitztümer */
	Item* inventory;
	unsigned int inventory_size;
 } Spawn;
 
 /* Wenn ein Spawn auf einein anderen trifft, gucke, was hier passiert (schubsen/töten/sprechen/...) */
 void spawn_spawn_collision(Spawn*, Spawn*);
 /* Deckt die unerkundete Gegend um einen Spawn herum auf. */
 void explore_area(Spawn*);
  /* Der Spawn ist am Zug, etwas zu tun. */
 void spawn_action(Spawn*);
 
 #endif