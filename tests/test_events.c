#include "../src/globals.h"
#include "../src/map.h"
#include "../src/map_loader.h"
#include "../src/action.h"
#include "../src/tile.h"

#define TEST_MAP "lonely"
#define NUM_MOVEMENTS 100
#define PRINT_ALL

int main() {
	int counter=0, num_moves=0, num_collisions=0;
	Map* map = NULL;
	Spawn* player = NULL;
	
	map = load_map(TEST_MAP);
	
	if(map == NULL) {
		fprintf(stderr, "!! Karte nicht gefunden.\n");
		goto summary;
	}
	unsigned int num_tiles=map->x*map->y;
	
	player = get_player_spawn(map);
	if(player==NULL){
		fprintf(stderr, "!! Kein Spieler auf der Karte.\n");
		goto summary;
	}

	#ifdef PRINT_ALL
	printf("OK Map geladen\n");
	#endif

	KeyAction action;

	/*zufällige Aktionen*/
	for(;counter<NUM_MOVEMENTS;counter++){
		int dir=rand()%4, should_move;
		switch(dir){
		case 0:
			action=UP;
			/*prüfen, ob in der Richtung überhaupt noch was ist*/
			if(player->y>0 && num_tiles<map->x*(player->y-1)+player->x){
				fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nicht am Rand der Karte, aber keine Tiles in Laufrichtung.\n", counter);
				goto summary;
			}
			if(player->y==0)
				should_move=0;
			else
				should_move=tile_can_walk(map->tiles[map->x*(player->y-1)+player->x]);
			break;
		case 1:
			action=DOWN;
			if(player->y<map->y-1 && num_tiles<map->x*(player->y+1)+player->x){
				fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nicht am Rand der Karte, aber keine Tiles in Laufrichtung.\n", counter);
				goto summary;
			}
			if(player->y==map->y-1)
				should_move=0;
			else
				should_move=tile_can_walk(map->tiles[map->x*(player->y+1)+player->x]);
			break;	
		case 2:
			action=LEFT;
			if(player->x>0 && num_tiles<map->x*player->y+player->x-1){
				fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nicht am Rand der Karte, aber keine Tiles in Laufrichtung.\n", counter);
				goto summary;
			}
			if(player->x==0)
				should_move=0;
			else
				should_move=tile_can_walk(map->tiles[map->x*player->y+player->x-1]);
			break;	
		case 3:
			action=RIGHT;
			if(player->x<map->x-1 && num_tiles<map->x*player->y+player->x+1){
				fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nicht am Rand der Karte, aber keine Tiles in Laufrichtung.\n", counter);
				goto summary;
			}
			if(player->x==map->x-1)
				should_move=0;
			else
				should_move=tile_can_walk(map->tiles[map->x*player->y+player->x+1]);
			break;
		}

		unsigned int old_x=player->x, old_y=player->y;
		/*Event ausführen und Resultat prüfen*/
		process_event(action, map);
		/*sollte sich bewegen*/
		if(should_move){
			switch(dir){
			case 0:
				#ifdef PRINT_ALL
				printf("II Naechste Aktion: Bewegung nach oben...\n");
				#endif
				if(old_x!=player->x || old_y!=player->y+1){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Bewegung nach oben nicht die richtige Position erreicht.\n", counter);
					goto summary;
				}
				if(player->direction!=NORTH){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Bewegung nach oben nicht die richtige Laufrichtung gesetzt.\n", counter);
					goto summary;
				}
				break;
			case 1:
				#ifdef PRINT_ALL
				printf("II Naechste Aktion: Bewegung nach unten...\n");
				#endif
				if(old_x!=player->x || old_y!=player->y-1){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Bewegung nach unten nicht die richtige Position erreicht.\n", counter);
					goto summary;
				}
				if(player->direction!=SOUTH){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Bewegung nach unten nicht die richtige Laufrichtung gesetzt.\n", counter);
					goto summary;
				}
				break;
			case 2:
				#ifdef PRINT_ALL
				printf("II Naechste Aktion: Bewegung nach links...\n");
				#endif
				if(old_y!=player->y || old_x!=player->x+1){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Bewegung nach links nicht die richtige Position erreicht.\n", counter);
					goto summary;
				}
				if(player->direction!=WEST){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Bewegung nach links nicht die richtige Laufrichtung gesetzt.\n", counter);
					goto summary;
				}
				break;
			case 3:
				#ifdef PRINT_ALL
				printf("II Naechste Aktion: Bewegung nach rechts...\n");
				#endif
				if(old_y!=player->y || old_x!=player->x-1){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Bewegung nach rechts nicht die richtige Position erreicht.\n", counter);
					goto summary;
				}
				if(player->direction!=EAST){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Bewegung nach rechts nicht die richtige Laufrichtung gesetzt.\n", counter);
					goto summary;
				}
				break;
			}
			num_moves++;
			#ifdef PRINT_ALL
			printf("OK Bewegung korrekt\n");
			#endif
		}
		/*sollte stehenbleiben*/
		else{
			switch(dir){
			case 0:
				#ifdef PRINT_ALL
				printf("II Naechste Aktion: Kollision nach oben...\n");
				#endif
				if(old_x!=player->x || old_y!=player->y){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Kollision nach oben hat sich der Spieler bewegt.\n", counter);
					goto summary;
				}
				if(player->direction!=NORTH){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Kollision nach oben nicht die richtige Laufrichtung gesetzt.\n", counter);
					goto summary;
				}
				break;
			case 1:
				#ifdef PRINT_ALL
				printf("II Naechste Aktion: Kollision nach unten...\n");
				#endif
				if(old_x!=player->x || old_y!=player->y){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Kollision nach unten hat sich der Spieler bewegt.\n", counter);
					goto summary;
				}
				if(player->direction!=SOUTH){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Kollision nach unten nicht die richtige Laufrichtung gesetzt.\n", counter);
					goto summary;
				}
				break;
			case 2:
				#ifdef PRINT_ALL
				printf("II Naechste Aktion: Kollision nach links...\n");
				#endif
				if(old_x!=player->x || old_y!=player->y){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Kollision nach links hat sich der Spieler bewegt.\n", counter);
					goto summary;
				}
				if(player->direction!=WEST){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Kollision nach links nicht die richtige Laufrichtung gesetzt.\n", counter);
					goto summary;
				}
				break;
			case 3:
				#ifdef PRINT_ALL
				printf("II Naechste Aktion: Kollision nach rechts...\n");
				#endif
				if(old_x!=player->x || old_y!=player->y){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Kollision nach rechts hat sich der Spieler bewegt.\n", counter);
					goto summary;
				}
				if(player->direction!=EAST){
					fprintf(stderr, "!! Nach %d korrekten Aktionen:\n!! Nach einer Kollision nach rechts nicht die richtige Laufrichtung gesetzt.\n", counter);
					goto summary;
				}
				break;
			}
			num_collisions++;
			#ifdef PRINT_ALL
			printf("OK Kollision korrekt\n");
			#endif
		}
	}
	
	summary:
	printf("II %d/%d Aktionen korrekt, davon %d Bewegungen und %d Kollisionen.\n", counter, NUM_MOVEMENTS, num_moves, num_collisions);

	flush_map(map);
	if(counter==NUM_MOVEMENTS-1)
		return 0;
	else
		return 1;
}
