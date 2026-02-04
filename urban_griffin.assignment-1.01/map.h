typedef struct map {
	int width;
	int height;
	char **cells;
} map;

int map_init(map *m, int w, int h);
int map_destroy(map *m);
int map_generate(map *m);
int map_print(map *m);
