#ifndef WORLD_H
#define WORLD_H

#include "map.h"
#include "character.h"

#define WORLD_SIZE_X 401
#define WORLD_SIZE_Y 401

typedef struct world {
	map *maps[WORLD_SIZE_Y][WORLD_SIZE_X]; // array of maps in world
	int cur_idx_x;
	int cur_idx_y;

	character_t pc;
	int hiker_dist[MAP_HEIGHT][MAP_WIDTH]; // distance map for hiker
	int rival_dist[MAP_HEIGHT][MAP_WIDTH]; // distance map for rival
} world;

int world_init(world *w);
int world_destroy(world *w);

map* world_get_map(world *w, int x, int y); // returns map at x y

#endif
