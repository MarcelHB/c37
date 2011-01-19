/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * item.c
 */
 
 #include "globals.h"
 #include "memory.h"
 #include "item.h"
 
 /*--------------------------------------------------------------------------*/
 void apply_item_defaults(Item* item) {
	int i;
	int elements = sizeof(item_defaults)/sizeof(item_defaults[0]);
	
	for(i = 0; i < elements; ++i) {
		if(item_defaults[i].type == item->type) {
			item->name = (char*)ex_calloc(strlen(item_defaults[i].name) + 1, 1);
			strcpy(item->name, item_defaults[i].name);
		
			item->color = item_defaults[i].color;
			item->value = item_defaults[i].value;
			item->weight = item_defaults[i].weight;
			break;
		}
	}
 }
 
 /*--------------------------------------------------------------------------*/
 void create_item_properties(Item* item) {
	if(item->properties == NULL) {
		/* Wand */
		if(item->type == ITEM_TYPE_HEALTH_POTION) {
			HealthPotionProperties hp_props = {
				/*.capacity = */20
			};
			item->properties = malloc(sizeof(HealthPotionProperties));
			*(HealthPotionProperties*)item->properties = hp_props;
		}
	}
 }