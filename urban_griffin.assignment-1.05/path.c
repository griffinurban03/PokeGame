#include <stdio.h>

#include "path.h"
#include "heap.h"
#include "character.h"
#include "world.h" 

/*
 * Dijkstra implementation for a PC/NPC/Path
 */
static void dijkstra(map *m, character_t *pc, movement_type_t mtype, int dist_grid[MAP_HEIGHT][MAP_WIDTH]) {
	int x, y;
	
	// Initialize all distances to infinity
	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {
			dist_grid[y][x] = INF;
		}
	}

	// The PC's location is distance 0
	dist_grid[pc->y][pc->x] = 0;

	// Init Priority Queue
	heap_t h;
	heap_init(&h, MAP_WIDTH * MAP_HEIGHT); 
	heap_push(&h, pc->x, pc->y, 0);

	// Init 8-way offsets
	int dx[8] = { 0,  1,  1,  1,  0, -1, -1, -1};
	int dy[8] = {-1, -1,  0,  1,  1,  1,  0, -1};

	heap_node_t n;
	
	while (heap_pop(&h, &n)) {
		// Lazy deletion check: Skip if we already found a shorter path
		if (n.distance > dist_grid[n.y][n.x]) continue;

		// Check all 8 neighbors using offsets
		for (int i = 0; i < 8; i++) {
			int nx = n.x + dx[i];
			int ny = n.y + dy[i];

			// Ensure neighbor is within map bounds
			if (nx >= 0 && nx < MAP_WIDTH && ny >= 0 && ny < MAP_HEIGHT) {
				
				// Can the NPC even stand on this neighbor tile
				if (character_get_cost(mtype, m->cells[ny][nx]) == INF) continue;

				// Get cost of moving onto this neighbor tile and calculate new distance
				int edge_weight = character_get_cost(mtype, m->cells[n.y][n.x]);
				
				if (edge_weight != INF) {
					int new_dist = n.distance + edge_weight;
					
					// If this new path is shorter than the previously recorded path
					if (new_dist < dist_grid[ny][nx]) {
						dist_grid[ny][nx] = new_dist;
						heap_push(&h, nx, ny, new_dist); 
					}
				}
			}
		}
	}
	heap_destroy(&h);
}

void pathfind_build_distance_map(world *w, map *m) {
	dijkstra(m, &w->pc, move_hiker, w->hiker_dist);
	dijkstra(m, &w->pc, move_land, w->rival_dist);
}

void pathfind_print_distance_map(world *w, character_type_t type) {
	int x, y;
	
	int (*dist_grid)[MAP_WIDTH] = (type == char_hiker) ? w->hiker_dist : w->rival_dist;

	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {
			if (dist_grid[y][x] == INF) {
				printf("   "); 
			} else {
				printf("%02d ", dist_grid[y][x] % 100); 
			}
		}
		printf("\n");
	}
}