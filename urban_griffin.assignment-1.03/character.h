#ifndef CHARACTER_H
#define CHARACTER_H

#include <limits.h>
#include "map.h"

#define INF INT_MAX

typedef enum character_type {
	char_pc,
	char_hiker,
	char_rival,
	char_swimmer,
	char_other,
	CHAR_TYPE_COUNT
} character_type_t;

typedef struct character {
	character_type_t type;
	char symbol;
	int x;
	int y;
} character_t;

struct map;

int character_place_pc(character_t *pc, struct map *m);

int character_place_npc(character_t *npc, struct map *m, character_type_t type, char symbol);

int character_get_cost(character_type_t char_type, terrain_type_t ter_type);

#endif
