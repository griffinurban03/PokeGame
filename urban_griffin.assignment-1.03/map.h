#ifndef MAP_H
#define MAP_H

typedef struct map {
	int width;
	int height;
	char **cells;
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

#endif
