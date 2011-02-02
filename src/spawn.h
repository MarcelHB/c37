/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * spawn.h
 */

 #ifndef _C37_SPAWN_H
 #define _C37_SPAWN_H
 
#include <stdbool.h>
 #include "item.h"

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
    /* (max) HPs */
    unsigned int max_hp, hp;
    /* Polymorphismusersatz für den eig. Typ */
    unsigned int type;
    /* spez. Eigenschaften des Spawns */
    void* properties;
    /* Besitztümer */
    Item** inventory;
    unsigned int inventory_size;
    unsigned int selected_item;
 } Spawn;
 
 /* verwendete Spawn-type identifier */
 #define SPAWN_TYPE_INVALID                0xFFFFFFFF
 #define SPAWN_TYPE_PLAYER                0x00000000
 #define SPAWN_TYPE_HOUND                0x00000001
 
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
 static const struct SpawnDefault spawn_defaults[2] = {
    {SPAWN_TYPE_PLAYER, "Spieler", '@', 0, 1, 100, 100},
    {SPAWN_TYPE_HOUND, "Bluthund", 'h', 1, 0, 50, 50}
 };

typedef struct HoundProperties {
    unsigned int targetx, targety;
    bool has_target;
} HoundProperties;
 
 /* Gibt reservierten Speicher eines Spawns frei */
 void free_spawn(Spawn*);
 
 /* Helper */
 void apply_spawn_defaults(Spawn*);
 
 #endif
