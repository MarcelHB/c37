/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * action.h
 */

#ifndef _C37_ACTION_H
#define _C37_ACTION_H

#include "map.h"
#include "spawn.h"
#include "item.h"
#include "tile.h"

typedef enum {UP, DOWN, LEFT, RIGHT, ACTION, USE, NEXT_MSG, PREV_MSG, NEXT_ITEM, PREV_ITEM, REST, INVALID} KeyAction;

/*aktualisiert eine Map gemäß einem Event*/
void process_event(KeyAction, Map*);

/* war mal spawn.h, dann map.h */
/* Wenn ein Spawn auf einein anderen trifft, gucke, was hier passiert (schubsen/töten/sprechen/...) */
void spawn_spawn_collision(Spawn*, Spawn*, Map*);
/* Der Spawn ist am Zug, etwas zu tun. */
void spawn_action(Spawn*, Map*);
/* Spwan führt die Action eines Items aus (Trank trinken...) */
void spawn_uses_item(Spawn*, Item*, Map*);

/* war mal tile.h, dann map.h */
/* Wenn ein Spawn auf ein Tile trifft, gucke, was hier passiert (rennt gegen Wand/stirbt/sammelt auf/...) */
void spawn_tile_collision(Spawn*, Tile*, Map*);
/* irgendwas wird getoggelt (Tür auf, zu ...) */
void toggle_tile(Tile*, Map*);

void spawn_run_ai (Spawn *, Map *);

#endif
