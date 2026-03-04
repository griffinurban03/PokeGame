#include "path.h"
#include "heap.h"
#include "character.h"
#include <stdio.h>

/*
 * Dijkstra implementation for a PC/NPC
 */
static void dijkstra(map *m, character_t *pc, character_type_t type, int **dist_grid) {
	int x, y;
	
	// Initialize all distances to infinity
	for (y = 0; y < m->height; y++) {
		for (x = 0; x < m->width; x++) {
			dist_grid[y][x] = INF;
		}
	}

	// The PC's location is distance 0
	dist_grid[pc->y][pc->x] = 0;

	// Init Priority Queue
	heap_t h;
	heap_init(&h, m->width * m->height); 
	heap_push(&h, pc->x, pc->y, 0);

	// Init 8-way offsets
	int dx[8] = { 0,  1,  1,  1,  0, -1, -1, -1};
	int dy[8] = {-1, -1,  0,  1,  1,  1,  0, -1};

	heap_node_t n;
	
	// Process heap
	while (heap_pop(&h, &n)) {
		// Lazy deletion check: Skip if we already found a shorter path to this cell
		if (n.distance > dist_grid[n.y][n.x]) continue;

		// Check all 8 neighbors using offsets
		for (int i = 0; i < 8; i++) {
			int nx = n.x + dx[i];
			int ny = n.y + dy[i];

			// Ensure neighbor is within map bounds
			if (nx >= 0 && nx < m->width && ny >= 0 && ny < m->height) {
				// Get the terrain cost for the NPC to step onto this tile
				int cost = character_get_cost(type, m->cells[ny][nx]);
				
				// If the terrain is passable
				if (cost != INF) {
					int new_dist = n.distance + cost;
					
					// If this new path is shorter than the previously recorded path
					if (new_dist < dist_grid[ny][nx]) {
						dist_grid[ny][nx] = new_dist;
						heap_push(&h, nx, ny, new_dist); // Push updated distance
					}
				}
			}
		}
	}
	heap_destroy(&h);
}

void pathfind_build_distance_map(map *m, character_t *pc) {
	dijkstra(m, pc, char_hiker, m->hiker_dist);
	dijkstra(m, pc, char_rival, m->rival_dist);
}

void pathfind_print_distance_map(map *m, character_type_t type) {
	int x, y;
	int **dist_grid = (type == char_hiker) ? m->hiker_dist : m->rival_dist;

	for (y = 0; y < m->height; y++) {
		for (x = 0; x < m->width; x++) {
			if (dist_grid[y][x] == INF) {
				printf("   "); 
			} else {
				printf("%02d ", dist_grid[y][x] % 100); 
			}
		}
		printf("\n");
	}
}