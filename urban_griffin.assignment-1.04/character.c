#include <stdlib.h>

#include "character.h"
#include "map.h"
#include "world.h"

/*
* Lookup table for movement costs of every PC/NPC
*/
static const int movement_cost[MOVE_TYPE_COUNT][TERRAIN_TYPE_COUNT] = {
	[move_pc] = { // Covers player character
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
	[move_hiker] = { // Covers any unit that is able to move over mountain/forests
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
	[move_land] = { // Covers Rival, and land-based Pacers/Wanderers/etc
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
	[move_water] = { // Covers water-based Pacers/Wanderers/etc (Swimmers)
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
	}
};

int character_get_cost(movement_type_t mt, terrain_type_t tt) 
{
	if (mt < 0 || mt >= MOVE_TYPE_COUNT) return INF;
	if (tt < 0 || tt >= TERRAIN_TYPE_COUNT) return INF;
	
	return movement_cost[mt][tt];
}

/*
 * Place PC (Player Character) on a random path cell that is not a gate
 */
int character_place_pc(character_t *pc, map *m)
{
	int placed = 0;

	pc->type = char_pc;
	pc->mtype = move_pc;
	pc->symbol = '@';

	while (!placed) {
		int rx = (rand() % (m->width-2)) + 1;
		int ry = (rand() % (m->height-2)) + 1;

		if (m->cells[ry][rx] == ter_path) {
			pc->x = rx;
			pc->y = ry;
			placed = 1;
		}
	}
	return 0;
}

/*
 * Places an NPC on a random cell that is valid for their movement parameters
 */
int character_place_npc(character_t *npc, map *m, character_t *pc, character_type_t type, movement_type_t mtype, char symbol)
{
	int placed = 0;
	int attempts = 0;
	
	npc->type = type;
	npc->mtype = mtype;
	npc->symbol = symbol;
	
	while (!placed && attempts < 1000) { // Avoid infinite loop
		int rx = (rand() % (m->width - 2)) + 1;
		int ry = (rand() % (m->height - 2)) + 1;

		// Check if NPC can actually exist on this terrain AND not occupied already
		if (character_get_cost(mtype, m->cells[ry][rx]) != INF && m->cells[ry][rx] != ter_mart && m->cells[ry][rx] != ter_center && m->cmap[ry][rx] == NULL && (rx != pc->x || ry != pc->y)) {
			npc->x = rx;
			npc->y = ry;
			m->cmap[ry][rx] = npc;
			placed = 1;
		}
		attempts++;
	}

	if (!placed) {
		npc->mtype = move_land; // Force them to walk on land
		while (!placed) {
			int rx = (rand() % (m->width - 2)) + 1;
			int ry = (rand() % (m->height - 2)) + 1;

			if (character_get_cost(npc->mtype, m->cells[ry][rx]) != INF && m->cells[ry][rx] != ter_mart && m->cells[ry][rx] != ter_center && m->cmap[ry][rx] == NULL && (rx != pc->x || ry != pc->y)) {
				
				npc->x = rx;
				npc->y = ry;
				m->cmap[ry][rx] = npc;
				placed = 1;
			}
		}
	}

	// Give wandering NPC a random starting direction
	if (type == char_pacer || type == char_wanderer || type == char_explorer) {
		do {
			npc->dir_x = (rand() % 3) - 1; // -1, 0, or 1
			npc->dir_y = (rand() % 3) - 1;
		} while (npc->dir_x == 0 && npc->dir_y == 0);
	} else {
		npc->dir_x = 0;
		npc->dir_y = 0;
	}
	return 0;
}

/*
 * Determines the next coordinates for an NPC based on its specific AI behavior.
 */
void character_get_next_pos(world *w, map *m, character_t *c, int *next_x, int *next_y) {
	// Default to not moving
	*next_x = c->x;
	*next_y = c->y;

	// Sentries don't move
	if (c->type == char_sentry || c->type == char_pc) return;

	int nx, ny;

	// Hikers and Rivals
	if (c->type == char_hiker || c->type == char_rival) {
		int min_dist = INF;
		int (*dist_map)[MAP_WIDTH] = (c->type == char_hiker) ? w->hiker_dist : w->rival_dist;

		// Check all 8 neighbors
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				if (dx == 0 && dy == 0) continue;
				
				nx = c->x + dx;
				ny = c->y + dy;

				// Bounds check
				if (nx > 0 && nx < MAP_WIDTH - 1 && ny > 0 && ny < MAP_HEIGHT - 1) {
					
					if (nx == w->pc.x && ny == w->pc.y) {
						// TODO put check if hit player
						continue; 
					}

					// Collision check for cmap
					if (m->cmap[ny][nx] == NULL && character_get_cost(c->mtype, m->cells[ny][nx]) != INF) {
						if (dist_map[ny][nx] < min_dist) {
							min_dist = dist_map[ny][nx];
							*next_x = nx;
							*next_y = ny;
						}
					}
				}
			}
		}
		return;
	}

	// Pacers, Wanderers, and Explorers
	nx = c->x + c->dir_x;
	ny = c->y + c->dir_y;

	bool obstacle = false;
	if (nx <= 0 || nx >= MAP_WIDTH - 1 || ny <= 0 || ny >= MAP_HEIGHT - 1) {
		obstacle = true;
	} 
	else if (m->cmap[ny][nx] != NULL) {
		obstacle = true;
	} 
	else if (nx == w->pc.x && ny == w->pc.y) {
		obstacle = true; 
		// TODO if hit player
	}
	else if (character_get_cost(c->mtype, m->cells[ny][nx]) == INF) {
		obstacle = true; // Impassable terrain
	}

	// Wanderers can't leave current terrain type
	if (c->type == char_wanderer && !obstacle) {
		if (m->cells[ny][nx] != m->cells[c->y][c->x]) obstacle = true;
	}

	if (obstacle) {
		// Pacer turns around
		if (c->type == char_pacer) {
			c->dir_x *= -1;
			c->dir_y *= -1;
		}
		// Wanderer/Explorer pick random direction
		else {
			do {
				c->dir_x = (rand() % 3) - 1;
				c->dir_y = (rand() % 3) - 1;
			} while (c->dir_x == 0 && c->dir_y == 0);
		}
	} else {
		*next_x = nx;
		*next_y = ny;
	}
}
