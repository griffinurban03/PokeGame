#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "map.h"
#include "world.h"

#define WIDTH 80
#define HEIGHT 21

int main(int argc, char *argv[])
{
	srand(time(NULL));

	world w;
	world_init(&w);

	// Starting location
	int cur_x = 200;
	int cur_y = 200;

	map *current_map = world_get_map(&w, cur_x, cur_y);
	map_print(current_map);
	printf("Current location: %d x %d\n", cur_x, cur_y);

	bool running = true;
	char c[20];

	while(running) {
		printf("Enter a Command: ");

		if (fgets(c, sizeof(c), stdin) == NULL) break;


		int new_x = cur_x;
		int new_y = cur_y;
		int fly_x, fly_y;

		// Check first for f x y
		if (sscanf(c, "f %d %d", &fly_x, &fly_y) == 2) {
			if (fly_x >= 0 && fly_x < WORLD_SIZE_X && fly_y >= 0 && fly_y < WORLD_SIZE_Y) {
				new_x = fly_x;
				new_y = fly_y;
			} else {
				printf("Coordinates out of bounds\n");
				continue;
			}
		} else {
			switch (c[0]) {
				case 'n':
					printf("Moved North\n");
					new_y--;
					break;
				case 's':
					printf("Moved South\n");
					new_y++;
					break;
				case 'e':
					printf("Moved East\n");
					new_x++;
					break;
				case 'w':
					printf("Moved West\n");
					new_x--;
					break;
				case 'q':
					printf("Quitting...\n");
					running = false;
					break;
				case '\n':
					break;
				default:
					printf("Enter a valid command: n | e | s | w | q | f x y\n");
					break;
		}

	}

		if (running && (new_x != cur_x || new_y != cur_y)) {
			if (new_x < 0 || new_x >= WORLD_SIZE_X || new_y < 0 || new_y >= WORLD_SIZE_Y) {
				printf("Cannot move: edge of world\n");
			} else {
				cur_x = new_x;
				cur_y = new_y;
				current_map = world_get_map(&w, cur_x, cur_y);
				// Clear Terminal

				map_print(current_map);
				printf("Current coordinates: (%d, %d)\n", cur_x - 200, cur_y - 200);
			}
		}
	}

	world_destory(&w);
	return 0;
}
