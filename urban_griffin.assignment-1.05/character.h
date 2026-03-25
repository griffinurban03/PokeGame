#ifndef CHARACTER_H
#define CHARACTER_H

#include <limits.h>
#include "map.h"

#define INF INT_MAX

typedef enum movement_type {
	move_pc,
	move_hiker,
	move_land,
	move_water,
	MOVE_TYPE_COUNT
} movement_type_t;

typedef enum character_type {
	char_pc,
	char_hiker,
	char_rival,
	char_pacer,
	char_wanderer,
	char_sentry,
	char_explorer,
	CHAR_TYPE_COUNT
} character_type_t;

typedef struct character {
	character_type_t type;
	movement_type_t mtype;
	char symbol;
	int x;
	int y;
	int dir_x;
	int dir_y;
	int defeated;
} character_t;

struct map;
struct world;

int character_place_pc(character_t *pc, struct map *m);

int character_place_npc(character_t *npc, struct map *m, character_t *pc, character_type_t type, movement_type_t mtype, char symbol);

int character_get_cost(movement_type_t mtype, terrain_type_t ter_type);

void character_get_next_pos(struct world *w, struct map *m, character_t *c, int *next_x, int *next_y);

#endif
