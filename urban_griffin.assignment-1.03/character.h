#ifndef CHARACTER_H
#define CHARACTER_H

#include <limits.h>

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

#endif
