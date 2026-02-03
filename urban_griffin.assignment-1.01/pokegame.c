#include <stdio.h>
#include "map.h"

int main(int argc, char *argv[])
{
	map m;
	if (map_init(&m, 80, 21)) {
		return -1;
	}

	printf("w: %d, h: %d", m.width, m.height);
	return 0;
}
