/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * action.h
 */

#ifndef _C37_ACTION_H
#define _C37_ACTION_H

#include "SDL.h"
#include "map.h"

/*aktualisiert eine Map gemäß einem Event*/
void process_event(SDL_Event*, Map*);

#endif
