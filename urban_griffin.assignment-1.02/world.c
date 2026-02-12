#include "world.h"
#include <stdlib.h>
#include <stdio.h>


/*
 * Initializes the world of HEIGHT x WIDTH  maps to be NULL
 */
int world_init(world *w) 
{
	int i, j;

	for (i = 0; i < WORLD_SIZE_Y; i++) {
		for (j = 0; j < WORLD_SIZE_X; j++) {
			w->maps[i][j] = NULL;
		}
	}
	return 0;

}


/*
 * Checks every map and frees it if it exists
 */
int world_destory(world *w)
{
	int i, j;

	for (i = 0; i < WORLD_SIZE_Y; i++) {
		for (j = 0; j < WORLD_SIZE_X; j++) {
			if (w->maps[i][j]) {
				map_destroy(w->maps[i][j]);
				free(w->maps[i][j]);
				w->maps[i][j] = NULL;
			}
		}
	}

	return 0;
}

map* world_get_map(world *w, int x, int y)
{
	// Check bounds
	if (x < 0 || x >= WORLD_SIZE_X || y < 0 || y >= WORLD_SIZE_Y) {
		return NULL;
	}

	// Check if NULL
	if (w->maps[y][x] != NULL) {
		return w->maps[y][x];
	}

	// generate if nothing exists
	w->maps[y][x] = malloc(sizeof(map));
	map_init(w->maps[y][x], 80, 21);

	// Check neighbors for gates
	// Set gates default to -1 for no gate
	int n = -1;
	int s = -1;
	int e = -1;
	int west = -1;

	if (y - 1 >= 0 && w->maps[y - 1][x]) {
		n = w->maps[y - 1][x]->south_gate[0];
	}

	if (y + 1 < WORLD_SIZE_Y && w->maps[y + 1][x]) {
		s = w->maps[y + 1][x]->north_gate[0];
	}

	if (x + 1 < WORLD_SIZE_X && w->maps[y][x + 1]) {
		e = w->maps[y][x + 1]->west_gate[1];
	}

	if (x - 1 >= 0 && w->maps[y][x - 1]) {
		west = w->maps[y][x - 1]->east_gate[1];
	}

	map_generate(w->maps[y][x], x, y, n, s, e, west);

	return w->maps[y][x];
}
