#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

/*
 * Enum definitions of all terrain types
 */
typedef enum terrain_t {
	ter_debug,
	ter_boulder,
	ter_tree,
	ter_path,
	ter_mart,
	ter_center,
	ter_grass,
	ter_clearing,
	ter_mountain,
	ter_forest,
	ter_water,
	ter_gate,
	TERRAIN_TYPE_COUNT
} terrain_type_t;

typedef struct map {
	int width;
	int height;
	terrain_type_t **cells;
	int north_gate[2];
	int south_gate[2];
	int east_gate[2];
	int west_gate[2];
} map;

int map_init(map *m, int w, int h);
int map_destroy(map *m);
int map_print(map *m);
int map_generate(map *m, int x, int y, int n, int s, int e, int w);

int map_generate_terrain(map *m);
int map_generate_borders(map *m);

int map_generate_paths(map *m, int n, int s, int e, int w);
int map_generate_pokeshops(map *m, int x, int y);

char map_get_terrain_char(terrain_type_t t);

#endif
