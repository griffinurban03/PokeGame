#include "character.h"
#include "map.h"
#include <stdlib.h>

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
