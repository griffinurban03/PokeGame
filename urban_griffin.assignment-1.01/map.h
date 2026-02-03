#IFNDEF
#DEFINE MAP_H

typedef struct map {
	int width;
	int height;
} map;

int map_init(map *m, int w, int h);
int map_destory(map *m);

#ENDIF
