#include <stdio.h>
#include "map.h"

#define WIDTH 80
#define HEIGHT 21

int main(int argc, char *argv[])
{
	map m;
	if (map_init(&m, WIDTH, HEIGHT)) {
		return -1;
	}

	map_generate(&m);

	printf("w: %d, h: %d\n", m.width, m.height);
	map_print(&m);
	map_destroy(&m);
	return 0;
}
