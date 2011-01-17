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
	unsigned int type;
	/* spez. Eigenschaften des Spawns */
	void* properties;
	/* Besitztümer */
	Item** inventory;
	unsigned int inventory_size;
 } Spawn;
 
 /* verwendete Spawn-type identifier */
 #define SPAWN_TYPE_INVALID				0xFFFFFFFF
 #define SPAWN_TYPE_PLAYER				0x00000000
 
 /* Zuordnungstabellen von type -> defaults */
 struct SpawnDefault {
	unsigned int type;
	char* name;
	char glyph;
	char npc;
	char humanoid;
	unsigned int max_hp;
	unsigned int hp;
 };
 
 /* lediglich Intialisierungswerte */
 const struct SpawnDefault spawn_defaults[1] = {
	{SPAWN_TYPE_PLAYER, "Spieler", '@', 0, 1, 100, 100}
 };
 
 /* Wenn ein Spawn auf einein anderen trifft, gucke, was hier passiert (schubsen/töten/sprechen/...) */
 void spawn_spawn_collision(Spawn*, Spawn*, Map*);
 /* Deckt die unerkundete Gegend um einen Spawn herum auf. */
 void explore_area(Spawn*, Map*);
  /* Der Spawn ist am Zug, etwas zu tun. */
 void spawn_action(Spawn*, Map*);
  /* Gibt reservierten Speicher eines Spawns frei */
 void free_spawn(Spawn*);
 
 /* Helper */
 void apply_spawn_defaults(Spawn*);
 
 #endif