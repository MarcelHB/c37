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

void
spawn_run_ai (Spawn *self, Map *map) {
    unsigned int nx = self->x, ny = self->y;
    Spawn *player = get_player_spawn(map);
    HoundProperties *props = self->properties;
    if (map_can_see(map, self->x, self->y, player->x, player->y)) {
        props->targetx = player->x;
        props->targety = player->y;
    }
    if (self->x == props->targetx && self->y == props->targety) {
        props->targetx = -1;
        props->targety = -1;
    }

    if ((1 == abs(player->x - self->x) && 0 == abs(player->y - self->y)) ||
        (0 == abs(player->x - self->x) && 1 == abs(player->y - self->y))) {
        /* adjacent, attack */
    } else if (-1 != props->targetx && -1 != props->targety) {
        int dx = props->targetx - self->x, dy = props->targety - self->y;
        if (abs(dx) >= abs(dy)) {
            if (0 < dx) {
                ++nx;
            } else {
                --nx;
            }
        } else {
            if (0 < dy) {
                ++ny;
            } else {
                --ny;
            }
        }
        /* Wir wollen nicht in andere Spawns reinlaufen. */
        Spawn *other = NULL;
        for (unsigned int ii = 0; ii < map->number_of_spawns; ++ii) {
            if (map->spawns[ii]->x == nx && map->spawns[ii]->y == ny) {
                other = map->spawns[ii];
            }
        }
        if (!tile_can_walk(map->tiles[nx * map->y + ny]) || NULL != other) {
            nx = self->x;
            ny = self->y;
        }
    } else {
        int dir = rand() % 4;
        switch (dir) {
            case 0: /* north */
                --ny;
                break;
            case 1: /* east */
                ++nx;
                break;
            case 2: /* south */
                ++ny;
                break;
            case 3: /* west */
                --nx;
                break;
        }
        /* Wir wollen nicht in andere Spawns reinlaufen. */
        Spawn *other = NULL;
        for (unsigned int ii = 0; ii < map->number_of_spawns; ++ii) {
            if (map->spawns[ii]->x == nx && map->spawns[ii]->y == ny) {
                other = map->spawns[ii];
            }
        }
        if (!tile_can_walk(map->tiles[nx * map->y + ny]) || NULL != other) {
            nx = self->x;
            ny = self->y;
        }
    }
    self->x = nx;
    self->y = ny;
}
