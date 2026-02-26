#include "character.h"
#include "map.h"
#include <stdlib.h>

/*
* Lookup table for movement costs of every PC/NPC
*/
static const int movement_cost[CHAR_TYPE_COUNT][TERRAIN_TYPE_COUNT] = {
	[char_pc] = {
		[ter_boulder] = INF,
		[ter_tree] = INF,
		[ter_path] = 10,
		[ter_mart] = 10,
		[ter_center] = 10,
		[ter_grass] = 20,
		[ter_clearing] = 10,
		[ter_mountain] = INF,
		[ter_forest] = INF,
		[ter_water] = INF,
		[ter_gate] = 10,
		[ter_debug] = INF
	},
	[char_hiker] = {
		[ter_boulder] = INF,
		[ter_tree] = INF,
		[ter_path] = 10,
		[ter_mart] = 50,
		[ter_center] = 50,
		[ter_grass] = 15,
		[ter_clearing] = 10,
		[ter_mountain] = 15,
		[ter_forest] = 15,
		[ter_water] = INF,
		[ter_gate] = INF,
		[ter_debug] = INF
	},
	[char_rival] = {
		[ter_boulder] = INF,
		[ter_tree] = INF,
		[ter_path] = 10,
		[ter_mart] = 50,
		[ter_center] = 50,
		[ter_grass] = 20,
		[ter_clearing] = 10,
		[ter_mountain] = INF,
		[ter_forest] = INF,
		[ter_water] = INF,
		[ter_gate] = INF,
		[ter_debug] = INF
	},
	[char_swimmer] = {
		[ter_boulder] = INF,
		[ter_tree] = INF,
		[ter_path] = INF,
		[ter_mart] = INF,
		[ter_center] = INF,
		[ter_grass] = INF,
		[ter_clearing] = INF,
		[ter_mountain] = INF,
		[ter_forest] = INF,
		[ter_water] = 7,
		[ter_gate] = INF,
		[ter_debug] = INF
	},
	[char_other] = {
		[ter_boulder] = INF,
		[ter_tree] = INF,
		[ter_path] = 10,
		[ter_mart] = 50,
		[ter_center] = 50,
		[ter_grass] = 20,
		[ter_clearing] = 10,
		[ter_mountain] = INF,
		[ter_forest] = INF,
		[ter_water] = INF,
		[ter_gate] = INF,
		[ter_debug] = INF
	}
};

int character_get_cost(character_type_t ct, terrain_type_t tt) 
{
	if (ct < 0 || ct >= CHAR_TYPE_COUNT) return INF;
	if (tt < 0 || tt >= TERRAIN_TYPE_COUNT) return INF;
	
	return movement_cost[ct][tt];
}

/*
 * Place PC (Player Character) on a random path cell that is not a gate
 */
int character_place_pc(character_t *pc, map *m)
{
	int placed = 0;

	pc->type = char_pc;
	pc->symbol = '@';

	while (!placed) {
		int rx = (rand() % (m->width-2)) + 1;
		int ry = (rand() % (m->height-2)) + 1;

		if (m->cells[ry][rx] == ter_path && m->cmap[ry][rx] == NULL) {
			pc->x = rx;
			pc->y = ry;
			m->cmap[ry][rx] = pc;
			placed = 1;
		}
	}
	return 0;
}
