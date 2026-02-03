#include "map.h"

int map_init(map *m, int w, int h)
{
	m->width = w;
	m->height = h;

	return 0;
}
