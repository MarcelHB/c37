/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * spawn.c
 */

 #include "globals.h"
 #include "memory.h"
 #include "spawn.h"
 
 /*--------------------------------------------------------------------------*/
 void apply_spawn_defaults(Spawn* spawn) {
    int i;
    int elements = sizeof(spawn_defaults)/sizeof(spawn_defaults[0]);
    
    for(i = 0; i < elements; ++i) {
        if(spawn_defaults[i].type == spawn->type) {
            spawn->name = (char*)ex_calloc(strlen(spawn_defaults[i].name) + 1, 1);
            strcpy(spawn->name, spawn_defaults[i].name);
            
            spawn->glyph = spawn_defaults[i].glyph;
            spawn->npc = spawn_defaults[i].npc;
            spawn->humanoid = spawn_defaults[i].humanoid;
            spawn->hp = spawn_defaults[i].hp;
            spawn->max_hp = spawn_defaults[i].max_hp;
            break;
        }
    }
    if (spawn->type == SPAWN_TYPE_HOUND || spawn->type == SPAWN_TYPE_ZOMBIE) {
        spawn->properties = ex_calloc(1, sizeof(HoundProperties));
    }
 }
 
 /*--------------------------------------------------------------------------*/
 void free_spawn(Spawn* spawn) {
    unsigned int i;
    free(spawn->name);
    free(spawn->id);
    for(i = 0; i < spawn->inventory_size; ++i) {
        free_item(spawn->inventory[i]);
    }
    free(spawn->properties);
    free(spawn->inventory);
    free(spawn);
 }
