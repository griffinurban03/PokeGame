#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "map.h"
#include "world.h"
#include "character.h"
#include "path.h"

void print_splashscreen() {
printf(" /$$$$$$$   /$$$$$$  /$$   /$$ /$$$$$$$$  /$$$$$$   /$$$$$$  /$$      /$$ /$$$$$$$$\n");
printf("| $$__  $$ /$$__  $$| $$  /$$/| $$_____/ /$$__  $$ /$$__  $$| $$$    /$$$| $$_____/\n");
printf("| $$  \\ $$| $$  \\ $$| $$ /$$/ | $$      | $$  \\__/| $$  \\ $$| $$$$  /$$$$| $$      \n");
printf("| $$$$$$$/| $$  | $$| $$$$$/  | $$$$$   | $$ /$$$$| $$$$$$$$| $$ $$/$$ $$| $$$$$   \n");
printf("| $$____/ | $$  | $$| $$  $$  | $$__/   | $$|_  $$| $$__  $$| $$  $$$| $$| $$__/   \n");
printf("| $$      | $$  | $$| $$\\  $$ | $$      | $$  \\ $$| $$  | $$| $$\\  $ | $$| $$      \n");
printf("| $$      |  $$$$$$/| $$ \\  $$| $$$$$$$$|  $$$$$$/| $$  | $$| $$ \\/  | $$| $$$$$$$$\n");
printf("|__/       \\______/ |__/  \\__/|________/ \\______/ |__/  |__/|__/     |__/|________/\n");
printf("\n");
}
                                                                                 
#define DO_DEBUG 1                                                                                 

/*
* UI print function that controls all printing to the terminal
*/
void print_ui(map *m, character_t *pc, int cx, int cy) {
	system("clear");
	
	map_print(m);
	printf("Current coordinates: (%d, %d)\n", cx - 200, cy - 200);

	pathfind_build_distance_map(m, pc);
#if DO_DEBUG
	printf("\nHiker Distance Map\n");
	pathfind_print_distance_map(m, char_hiker);

	printf("\nRival Distance Map\n");
	pathfind_print_distance_map(m, char_rival);
#endif
}


int main(int argc, char *argv[])
{
	srand(time(NULL));

	world w;
	world_init(&w);

	// Starting location
	int cur_x = 200;
	int cur_y = 200;

	// Clear the terminal
	system("clear");

	// Get map for center of world
	map *current_map = world_get_map(&w, cur_x, cur_y);

	// Instatiate player character and place on map
	character_t player;
	character_place_pc(&player, current_map);

	character_t hiker;
	character_place_npc(&hiker, current_map, char_hiker, 'h');

	character_t rival;
	character_place_npc(&rival, current_map, char_rival, 'r');

	// Print map
	print_ui(current_map, &player, cur_x, cur_y);

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
			if (fly_x >= (WORLD_SIZE_X / -2 - 1) && fly_x < (WORLD_SIZE_X / 2 + 1) && fly_y >= (WORLD_SIZE_Y / -2 - 1) && fly_y < (WORLD_SIZE_Y / 2 + 1)) {
				new_x = fly_x + 200;
				new_y = fly_y + 200;
			} else {
				printf("Coordinates out of bounds\n");
				continue;
			}
		} else {
			switch (c[0]) {
				case 'n': printf("Moved North\n"); new_y--; break;
				case 's': printf("Moved South\n"); new_y++; break;
				case 'e': printf("Moved East\n");  new_x++; break;
				case 'w': printf("Moved West\n");  new_x--; break;
				case 'q': printf("Quitting...\n"); running = false; break;
				case '\n': break;
				default: printf("Enter a valid command: n | e | s | w | q | f x y\n"); break;
			}
		}

		if (running && (new_x != cur_x || new_y != cur_y)) {
			if (new_x < 0 || new_x >= WORLD_SIZE_X || new_y < 0 || new_y >= WORLD_SIZE_Y) {
				printf("Cannot move: edge of world\n");
			} else {
				if (current_map->cmap[player.y][player.x] == &player) current_map->cmap[player.y][player.x] = NULL;
				if (current_map->cmap[hiker.y][hiker.x] == &hiker) current_map->cmap[hiker.y][hiker.x] = NULL;
				if (current_map->cmap[rival.y][rival.x] == &rival) current_map->cmap[rival.y][rival.x] = NULL;

				cur_x = new_x;
				cur_y = new_y;
				current_map = world_get_map(&w, cur_x, cur_y);

				// TODO - replace this later, will just spawn the player in a new spot when moving to a new map
				character_place_pc(&player, current_map);
				character_place_npc(&hiker, current_map, char_hiker, 'h');
				character_place_npc(&rival, current_map, char_rival, 'r');

				print_ui(current_map, &player, cur_x, cur_y);
			}
		}
	}

	world_destroy(&w);
	return 0;
}
