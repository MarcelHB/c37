#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "map.h"
#include "spawn.h"
#include "tile.h"

Spawn *
get_player_spawn (Map *self) {
    for (unsigned int ii = 0; ii < self->number_of_spawns; ++ii) {
        if (SPAWN_TYPE_PLAYER == self->spawns[ii]->type) {
            return self->spawns[ii];
        }
    }
    return NULL;
}

/* Überprüft ob eine Stelle auf einer Map von einer anderen aus sichtbar ist.
 * (fx, fy) -> (tx, ty)
 * Siehe: http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html (Step 1) */
bool
map_can_see (Map *ma, int fx, int fy, int tx, int ty) {
    if (fx >= ma->x || fx < 0 || fy >= ma->y || fy < 0 ||
        tx >= ma->x || tx < 0 || ty >= ma->y || ty < 0) return false;
    if (fx == tx && fy == ty) return true;
    int dx = tx - fx, dy = ty - fy;
    if (abs(dx) > abs(dy)) {
        float m = (float)dy / (float)dx;
        float b = fy - m * fx;
        dx = (dx < 0) ? -1 : 1;
        while ((dx < 0) ? fx > tx : fx < tx) {
            if (!tile_can_light(ma->tiles[fx * ma->y + (int)(m * fx + b)])) {
                return false;
            }
            fx += dx;
        }
    } else {
        float m = (float)dx / (float)dy;
        float b = fx - m*fy;
        dy = (dy < 0) ? -1 : 1;
        while ((dy < 0) ? fy > ty : fy < ty) {
            if (!tile_can_light(ma->tiles[(int)(m * fy + b) * ma->y + fx])) {
                return false;
            }
            fy += dy;
        }
    }
    return true;
}
