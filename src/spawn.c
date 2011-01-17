/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * spawn.c
 */

 #include "globals.h"
 
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
 }
