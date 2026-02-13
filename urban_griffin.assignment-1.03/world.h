#ifndef WORLD_H
#define WORLD_H

#include "map.h"

#define WORLD_SIZE_X 401
#define WORLD_SIZE_Y 401

typedef struct world {
	map *maps[WORLD_SIZE_Y][WORLD_SIZE_X]; // array of maps in world
	int cur_idx_x;
	int cur_idx_y;
} world;

int world_init(world *w);
int world_destory(world *w);

map* world_get_map(world *w, int x, int y); // returns map at x y

#endif
