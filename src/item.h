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
 
 /* Properties fuer Klunker/Waffe/Misc... */
 #include "items/health_potion.h"
 
 /* verwendete Tile-type identifier */
 #define ITEM_TYPE_INVALID                0xFFFFFFFF
 #define ITEM_TYPE_HEALTH_POTION        0x00000001
 #define ITEM_TYPE_KEY                  0x00000002
 #define ITEM_TYPE_DEAD_CAT                0x00000003
 #define ITEM_TYPE_SHOTGUN                 0x00000004
 #define ITEM_TYPE_SHOTGUN_AMMO            0x00000005
 
 /* Zuordnungstabellen von type -> defaults */
 /* "healthpotion" -> "Health Potition", blau, 3lbs, 5GM ... */
 struct ItemDefault {
    unsigned int type;
    char* name;
    unsigned long color;
    int weight;
    int value;
 };
 
 /* lediglich Intialisierungswerte */
 static const struct ItemDefault item_defaults[5] = {
    {ITEM_TYPE_HEALTH_POTION, "Heiltrank", 0xFF000000, 1, 10},
    {ITEM_TYPE_KEY, "Schluessel", 0xDDDDDD00, 1, 10},
    {ITEM_TYPE_DEAD_CAT, "Tote Katze", 0x6B532800, 5, 1},
    {ITEM_TYPE_SHOTGUN, "Schrotflinte", 0xAAAAAA00, 40, 100},
    {ITEM_TYPE_SHOTGUN_AMMO, "Schrotflintenmunition", 0xAC8B4C00, 2, 2}
 };
 
 /* Gibt reservierten Speicher eines Items frei */
 void free_item(Item*);
 
 /* Helper */
 void apply_item_defaults(Item*);
 void create_item_properties(Item*);
 
 #endif
