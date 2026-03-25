#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

#include "map.h"
#include "world.h"
#include "character.h"
#include "path.h"
#include "heap.h"

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
                                                                                 
#define DO_DEBUG 0                                                                               

/*
* UI print function that controls all printing to the terminal
* Now reworked to use NCURSES
*/
void print_ui(world *w, map *m, int cx, int cy) {
	clear(); // NCURSES use of System(clear)
	
	mvprintw(0, 0, "Current coordinates: (%d, %d)", cx - 200, cy - 200); // New line for current location
	
	pathfind_build_distance_map(w, m);
	
	map_print(m, &w->pc);

	mvprintw(22, 0, "Use vi keys or numpad to move.");
	mvprintw(23, 0, "Press Q to quit.");

	refresh();

#if DO_DEBUG
	printf("\nHiker Distance Map\n");
	pathfind_print_distance_map(w, char_hiker);

	printf("\nRival Distance Map\n");
	pathfind_print_distance_map(w, char_rival);
#endif
}


int main(int argc, char *argv[])
{
	srand(time(NULL));

	// NCURSES setup (lowkey hate the way it looks when lowercase so its uppercase now)
	initscr();   		// ncurses init
	raw();       		// Disable line buffering
	noecho();    	      	// Hide typed keys
	curs_set(0); 	      	// Hide cursor
	keypad(stdscr, TRUE); 	// Enable special keys
	start_color();        	// Enable color

	// Color definitions for objects init_pair(ID, FG COLOR, BG COLOR)
	init_pair(1, COLOR_GREEN, COLOR_BLACK);		// Nature?
	init_pair(2, COLOR_BLUE, COLOR_BLACK);		// Water
	init_pair(3, COLOR_WHITE, COLOR_BLACK);		// Mountains/Boulders
	init_Pair(4, COLOR_YELLOW, COLOR_BLACK);	// Paths/Gates
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);	// Buildings
	init_pair(6, COLOR_CYAN, COLOR_BLACK);		// PC
	init_pair(7, COLOR_RED, COLOR_BLACK);		// NPCS?

	int num_trainers = 10;
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "--numtrainers") == 0 || strcmp(argv[i], "-numtrainers") == 0) {
				if (i + 1 < argc) {
					num_trainers = atoi(argv[i + 1]);
					i++;
				}
			}
		}

	world w;
	world_init(&w);

	// Starting location
	int cur_x = 200;
	int cur_y = 200;


	// Get map for center of world
	map *current_map = world_get_map(&w, cur_x, cur_y);

	// Instatiate player character and place on map
	character_place_pc(&w.pc, current_map);

	character_t *npcs = malloc(num_trainers * sizeof(character_t));

	for (int i = 0; i < num_trainers; i++) {
		if (i == 0) {
			character_place_npc(&npcs[i], current_map, &w.pc, char_hiker, move_hiker, 'h');
		} else if (i == 1) {
			character_place_npc(&npcs[i], current_map, &w.pc, char_rival, move_land, 'r');
		} else {
			// Randomly select behavior
			int r = rand() % 6;
			character_type_t ctype;
			char sym;
			
			switch (r) {
				case 0: ctype = char_hiker; sym = 'h'; break;
				case 1: ctype = char_rival; sym = 'r'; break;
				case 2: ctype = char_pacer; sym = 'p'; break;
				case 3: ctype = char_wanderer; sym = 'w'; break;
				case 4: ctype = char_sentry; sym = 's'; break;
				case 5: ctype = char_explorer; sym = 'e'; break;
			}

			// Decide movement constraint (Swimmer vs Land)
			movement_type_t mtype = move_land; 
			
			// If it's a generic NPC, give it a 20% chance to be a swimmer
			if (ctype != char_hiker && ctype != char_rival) {
				if (rand() % 100 < 20) {
					mtype = move_water;
					// Note: Assignment says swimmer characters are typically 
					// represented by 'm' in some roguelikes, but we'll stick 
					// to the behavior letter as clarified by the instructor!
				}
			} else if (ctype == char_hiker) {
				mtype = move_hiker;
			}

			character_place_npc(&npcs[i], current_map, &w.pc, ctype, mtype, sym);
		}
	}

	// Initial distance map generation
	pathfind_build_distance_map(&w, current_map);
	print_ui(&w, current_map, cur_x, cur_y);

	// Initialize the Turn Queue (Heap)
	heap_t turn_heap;
	heap_init(&turn_heap, num_trainers + 1);

	// Push PC (seq_num 0 ensures PC wins ties)
	heap_push_character(&turn_heap, &w.pc, 0, 0);

	// Push all NPCs (seq_num i + 1)
	for (int i = 0; i < num_trainers; i++) {
		heap_push_character(&turn_heap, &npcs[i], 0, i + 1);
	}

	bool running = true;
	heap_node_t current_turn;

	// Turn-based Event Loop
	while (running) {
		if (!heap_pop(&turn_heap, &current_turn)) break;

		character_t *c = current_turn.c;
		int current_time = current_turn.distance;

		if (c->type == char_pc) {
			// PC Turn
			// The PC stands still for this assignment. 

			// Recalculate maps and draw frame
			pathfind_build_distance_map(&w, current_map);
			print_ui(&w, current_map, cur_x, cur_y);
			
			// Pause for 250ms (4 Frames Per Second)
			usleep(250000); 

			// Push PC back into queue
			int cost = character_get_cost(c->mtype, current_map->cells[c->y][c->x]);
			heap_push_character(&turn_heap, c, current_time + cost, current_turn.seq_num);
		} else {
			// NPC Turn
			int nx, ny;
			character_get_next_pos(&w, current_map, c, &nx, &ny);

			// Move the NPC in the cmap if coordinates changed
			if (nx != c->x || ny != c->y) {
				current_map->cmap[c->y][c->x] = NULL;
				c->x = nx;
				c->y = ny;
				current_map->cmap[c->y][c->x] = c;
			}

			// Push NPC back into queue with new time
			int cost = character_get_cost(c->mtype, current_map->cells[c->y][c->x]);
			heap_push_character(&turn_heap, c, current_time + cost, current_turn.seq_num);
		}
	}

	heap_destroy(&turn_heap);
	free(npcs); 
	world_destroy(&w);

	endwin(); // Closes ncurses and restores terminal

	return 0;
}
