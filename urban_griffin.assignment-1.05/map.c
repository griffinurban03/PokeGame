#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ncurses.h>

#include "map.h"
#include "character.h"
#include "heap.h"

/*
 * Terrain types and properties
 */
static const char terrain_char_map[TERRAIN_TYPE_COUNT] = {
	[ter_debug]    = '0' ,
	[ter_boulder]  = '%' ,
	[ter_tree]     = '^' ,
	[ter_path]     = '#' ,
	[ter_mart]     = 'M' ,
	[ter_center]   = 'C' ,
	[ter_grass]    = ':' , 
	[ter_clearing] = '.' , 
	[ter_mountain] = '%' ,
	[ter_forest]   = '^' ,
	[ter_water]    = '~' ,
	[ter_gate]     = '#' 
};

// Replacement for char method
char map_get_terrain_char(terrain_type_t t) {
	if (t < 0 || t >= TERRAIN_TYPE_COUNT) return '0';
	return terrain_char_map[t];
}

/*
* Initialize the map with given width and height. Creates a 2D array for the map cells with the '0' character.
*/
int map_init(map *m, int w, int h)
{
	int i, j;

	m->width = w;
	m->height = h;

	m->cells = malloc(h * sizeof(terrain_type_t *));
	m->cmap = malloc(h * sizeof(character_t **));

	for (i = 0; i < h; i++) {
		m->cells[i] = malloc(w * sizeof(terrain_type_t));
		m->cmap[i] = malloc(w * sizeof(character_t *));

		for (j = 0; j < w; j++) {
			m->cells[i][j] = ter_debug;
			m->cmap[i][j] = NULL;
		}
	}

	return 0;
}

/*
* Free the allocated memory for the map cells
*/
int map_destroy(map *m)
{
	int i;

	for (i = 0; i < m->height; i++) {
		free(m->cells[i]);
		free(m->cmap[i]);
	}
	free(m->cells);
	free(m->cmap);

	return 0;
}


/*
* Print the map to the console
*/
int map_print(map *m, character_t *pc)
{
	int i, j;
	
	for (i = 0; i < m->height; i++) {
		for (j = 0; j < m->width; j++) {
			char symbol;
			int color;
			
			if (pc != NULL && pc->x == j && pc->y == i) {
				symbol = pc->symbol;
				color = 6; // PC ID
			} 
			else if (m->cmap[i][j] != NULL) {
				symbol = m->cmap[i][j]->symbol;
				color = 7; // NPC ID
			} 
			else {
				terrain_type_t t = m->cells[i][j];
				symbol = map_get_terrain_char(t);

				switch(t) {
					case ter_tree:
					case ter_forest:
					case ter_grass:
					case ter_clearing:
						color = 1; break; // Grass/Nature ID
					case ter_water:
						color = 2; break; // Water ID
					case ter_boulder:
					case ter_mountain:
						color = 3; break; // Rock ID
					case ter_path:
					case ter_gate:
						color = 4; break; // Path ID
					case ter_mart:
					case ter_center:
						color = 5; break; // Shop ID
					default:
						color = 8; break; // Debug ID
				}

			}

			attron(COLOR_PAIR(color));
			mvaddch(i + 1, j, symbol);
			attroff(COLOR_PAIR(color));
		}
	}

	return 0;
}

/*
* Place % around the border of the map
*/
int map_generate_borders(map *m) {
	int x, y;

	for (x = 0; x < m->width; x++) {
		m->cells[0][x]             = ter_boulder;
		m->cells[m->height - 1][x] = ter_boulder;
	}
	for (y = 0; y < m->height; y++) {
		m->cells[y][0]             = ter_boulder;
		m->cells[y][m->width - 1]  = ter_boulder;
	}

	return 0;
}

void dijkstra_path(map *m, int start_x, int start_y, int end_x, int end_y) {
	int dist[MAP_HEIGHT][MAP_WIDTH];
	int px[MAP_HEIGHT][MAP_WIDTH];
	int py[MAP_HEIGHT][MAP_WIDTH];
	int x, y;

	// Initialize grids
	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {
			dist[y][x] = INF;
			px[y][x] = -1;
			py[y][x] = -1;
		}
	}

	dist[start_y][start_x] = 0;

	heap_t h;
	heap_init(&h, MAP_WIDTH * MAP_HEIGHT);
	heap_push(&h, start_x, start_y, 0);

	// 4-way movement for clean, blocky roads
	int dx[4] = { 0,  0, -1,  1 };
	int dy[4] = {-1,  1,  0,  0 };

	heap_node_t n;
	
	while (heap_pop(&h, &n)) {
		if (n.x == end_x && n.y == end_y) break; // Found destination!
		if (n.distance > dist[n.y][n.x]) continue;

		for (int i = 0; i < 4; i++) {
			int nx = n.x + dx[i];
			int ny = n.y + dy[i];

			// Check bounds
			if (nx >= 0 && nx < MAP_WIDTH && ny >= 0 && ny < MAP_HEIGHT) {
				
				// Prevent pathing along the world borders (unless it is the destination gate)
				if ((nx == 0 || nx == MAP_WIDTH - 1 || ny == 0 || ny == MAP_HEIGHT - 1) && 
				    !(nx == end_x && ny == end_y)) {
					continue;
				}

				int edge_weight = 100;
				switch(m->cells[ny][nx]) {
					case ter_path:     edge_weight = 5;  break; // Encourage merging
					case ter_clearing: edge_weight = 10; break;
					case ter_grass:    edge_weight = 20; break;
					case ter_tree:     edge_weight = 50; break;
					case ter_boulder:  edge_weight = 50; break;
					case ter_water:    edge_weight = 100; break;
					case ter_mountain: edge_weight = 100; break;
					default:           edge_weight = 50; break;
				}

				int new_dist = n.distance + edge_weight;
				if (new_dist < dist[ny][nx]) {
					dist[ny][nx] = new_dist;
					px[ny][nx] = n.x;
					py[ny][nx] = n.y;
					heap_push(&h, nx, ny, new_dist);
				}
			}
		}
	}
	heap_destroy(&h);

	// Reconstruct the path backwards from destination to start
	x = end_x;
	y = end_y;
	while (x != start_x || y != start_y) {
		m->cells[y][x] = ter_path;
		
		int prev_x = px[y][x];
		int prev_y = py[y][x];
		
		// Failsafe in case path is completely blocked
		if (prev_x == -1 || prev_y == -1) break; 
		
		x = prev_x;
		y = prev_y;
	}
	m->cells[start_y][start_x] = ter_path;
}

/*
* Generate NS and EW paths, set 4 border cells to #, excluding corners, randomly (unless there exists a neighbor loaded)
* Then, use dijkstra's to create paths between them, following edges between terrain types
*/
int map_generate_paths(map *m, int n, int s, int e, int w) {

	int north_x = (n == -1) ? (rand() % (m->width - 4)) + 3 : n;
	int south_x = (s == -1) ? (rand() % (m->width - 4)) + 3 : s;
	int east_y = (e == -1) ? (rand() % (m->height - 4)) + 3 : e;
	int west_y = (w == -1) ? (rand() % (m->height - 4)) + 3 : w;

	// Initialize gates to an invalid default (-1)
	m->north_gate[0] = -1; m->north_gate[1] = -1;
	m->south_gate[0] = -1; m->south_gate[1] = -1;
	m->east_gate[0]  = -1; m->east_gate[1]  = -1;
	m->west_gate[0]  = -1; m->west_gate[1]  = -1;

	// Place gates and store coordinates if not on a world edge (-2)
	if (n != -2) {
		m->cells[0][north_x] = ter_path;
		m->north_gate[0] = north_x;
		m->north_gate[1] = 0;
	}
	if (s != -2) {
		m->cells[m->height - 1][south_x] = ter_path;
		m->south_gate[0] = south_x;
		m->south_gate[1] = m->height - 1;
	}
	if (e != -2) {
		m->cells[east_y][m->width - 1] = ter_path;
		m->east_gate[0] = m->width - 1;
		m->east_gate[1] = east_y;
	}
	if (w != -2) {
		m->cells[west_y][0] = ter_path;
		m->west_gate[0] = 0;
		m->west_gate[1] = west_y;
	}

	// Create a midpoint for paths to intersect
	int mid_x = m->width / 2;
	int mid_y = m->height / 2;

	// Draw paths from valid gates to the midpoint
	if (n != -2) dijkstra_path(m, north_x, 0, mid_x, mid_y);
	if (s != -2) dijkstra_path(m, south_x, m->height - 1, mid_x, mid_y);
	if (e != -2) dijkstra_path(m, m->width - 1, east_y, mid_x, mid_y);
	if (w != -2) dijkstra_path(m, 0, west_y, mid_x, mid_y);

	// Fix to make sure center isnt overridden
	m->cells[mid_y][mid_x] = ter_path;

	// Fix to make sure NPCs wont path to gate tiles
	if (n != -2) m->cells[0][north_x] = ter_gate;
	if (s != -2) m->cells[m->height - 1][south_x] = ter_gate;
	if (e != -2) m->cells[east_y][m->width - 1] = ter_gate;
	if (w != -2) m->cells[west_y][0] = ter_gate;

	return 0;
}

/*
* Generate 2 2x2 pokeshops at random locations attatched to a pathway '#'
*/
int map_generate_pokeshops(map *m, int x, int y) {
	int d = abs(x - 200) + abs(y - 200);
	int probability = 0;

	if (d == 0) {
		probability = 100;
	} else if (d < 200) {
		probability = ((-45 * d) / 200) + 50;
	} else {
		probability = 5;
	}

	int has_center = (rand() % 100) < probability;
	int has_mart = (rand() % 100) < probability;

	if (d == 0) {
		has_center = 1;
		has_mart = 1;
	}
	
	// Place Pokemon Center if chosen
	if (has_center) {
        	int placed = 0;
			int attempts = 0;
        	while (!placed && attempts < 1000) { // Avoid infinite loop
            		int mx = rand() % (m->width - 4) + 2;
            		int my = rand() % (m->height - 4) + 2;

            		// Check adjacent to path
            		if ((m->cells[my-1][mx] == ter_path || m->cells[my+2][mx] == ter_path ||
                		 m->cells[my][mx-1] == ter_path|| m->cells[my][mx+2] == ter_path) && 
                		(m->cells[my][mx] != ter_mart && m->cells[my][mx] != ter_center) &&
				(m->cells[my][mx] != ter_path) &&
                		(m->cells[my][mx+1] != ter_path) && 
                		(m->cells[my+1][mx] != ter_path) && 
                		(m->cells[my+1][mx+1] != ter_path)) 
            		{
                		m->cells[my][mx] = ter_center;
                		m->cells[my][mx+1] = ter_center;
                		m->cells[my+1][mx] = ter_center;
                		m->cells[my+1][mx+1] = ter_center;
                		placed = 1;
            		}
					attempts++;
        	}
    	}

    	//Place PokeMart if chosen
    	if (has_mart) {
        	int placed = 0;
			int attempts = 0;
       		while (!placed && attempts < 1000) { // Avoid infinite loop
      			int mx = rand() % (m->width - 4) + 2;
 	        	int my = rand() % (m->height - 4) + 2;

            		if ((m->cells[my-1][mx] == ter_path || m->cells[my+2][mx] == ter_path ||
                 		m->cells[my][mx-1] == ter_path || m->cells[my][mx+2] == ter_path) && 
                		(m->cells[my][mx] != ter_center && m->cells[my][mx] != ter_mart) &&
                		(m->cells[my][mx] != ter_path) &&
				(m->cells[my][mx+1] != ter_path) && 
                		(m->cells[my+1][mx] != ter_path) && 
                		(m->cells[my+1][mx+1] != ter_path)) 
            		{
                		m->cells[my][mx] = ter_mart;
                		m->cells[my][mx+1] = ter_mart;
                		m->cells[my+1][mx] = ter_mart;
                		m->cells[my+1][mx+1] = ter_mart;
                		placed = 1;
            		}
					attempts++;
        	}
    	}



	return 0;
}

/*
 * Biome Generator
 */
static int generate_biome(map *m, terrain_type_t biome, terrain_type_t base, int fill_probability, int iterations)
{
	int x, y, i, nx, ny;

	terrain_type_t **temp = malloc(m->height * sizeof(terrain_type_t *));
	for (i = 0; i < m->height; i++) {
		temp[i] = malloc(m->width * sizeof(terrain_type_t));
	}

	// Random noise
	for (y = 0; y < m->height; y++) {
		for (x = 0; x < m->width; x++) {
			if (m->cells[y][x] == base && (rand() % 100) < fill_probability) {
				m->cells[y][x] = biome;
			}
		}
	}

	// Smooth passes
	for (i = 0; i < iterations; i++) {
		for (y = 0; y < m->height; y++) {
			for (x = 0; x < m->width; x++) {
				temp[y][x] = m->cells[y][x];
			}
		}

		for (y = 0; y < m->height; y++) {
			for (x = 0; x < m->width; x++) {
				if (temp[y][x] == biome || temp[y][x] == base) {
					int neighbors = 0;
					for (ny = y - 1; ny <= y + 1; ny++) {
						for (nx = x - 1; nx <= x + 1; nx++) {
							if (nx >= 0 && nx < m->width && ny >= 0 && ny < m->height) {
								if ((nx != x || ny != y) && temp[ny][nx] == biome) {
									neighbors++;
								}
							}
						}
					}
					if (neighbors >= 5) m->cells[y][x] = biome;
					else if (neighbors <= 3) m->cells[y][x] = base;
				}
			}
		}
	}

	for (i = 0; i < m->height; i++) free(temp[i]);
	free(temp);


	return 0;
}

/*
 * Places and scatters random aesthetic terrain details like trees or boulders
 */
int scatter_terrain_elements(map *m)
{
	// Place trees
	int placed_trees = 0;
	int attempts = 0;

	while (placed_trees < 10 && attempts < 1000) { // Avoid infinite loop
		int tx = rand() % m->width;
		int ty = rand() % m->height;
		terrain_type_t t = m->cells[ty][tx];

		if (t == ter_clearing || t == ter_grass) {
			m->cells[ty][tx] = ter_tree;
			placed_trees++;
		}
		attempts++;
	}

	// Place Boulders
	int placed_boulders = 0;
	attempts = 0;

	while (placed_boulders < 10 && attempts < 1000) { // Avoid infinite loop
		int bx = rand() % m->width;
		int by = rand() % m->height;
		terrain_type_t b = m->cells[by][bx];

		if (b == ter_clearing || b == ter_grass) {
			m->cells[by][bx] = ter_boulder;
			placed_boulders++;
		}
		attempts++;
	}

	return 0;
}

/*
 * Could be used later to generate some special terrain, like pregenerated ideas??
 */
int generate_special_terrain(map *m)
{	
	return 0;
}

/*
 * Generates trees around mountain terrain
 */
int generate_foothill_forests(map *m)
{
	int x, y;

	for (y = 0; y < m->height; y++) {
                for (x = 0; x < m->width; x++) {
                        if (m->cells[y][x] == ter_clearing) {
                                int next_to_mountain = 0;
                                for (int ny = y - 1; ny <= y + 1; ny++) {
                                        for (int nx = x - 1; nx <= x + 1; nx++) {
                                                if (nx >= 0 && nx < m->width && ny >= 0 && ny < m->height) {
                                                        if (m->cells[ny][nx] == ter_mountain) next_to_mountain = 1;
                                                }
                                        }
                                }
                                if (next_to_mountain && (rand() % 100) < 60) {
                                        m->cells[y][x] = ter_tree;
                                }
                        }
                }
        }

	
	return 0;
}


/*
* Generate a random map with different terrain types 
*/
int map_generate_terrain(map *m)
{
	int x, y;
	
	for (y = 0; y < m->height; y++) {
		for (x = 0; x < m->width; x++) {
			m->cells[y][x] = ter_clearing;
		}
	}

	// Biome layering - map, biome to be added, base to add upon, noise, smoothing passes
	generate_biome(m, ter_water, ter_clearing, 40, 100);
	generate_biome(m, ter_mountain, ter_clearing, 45, 6);
	generate_biome(m, ter_forest, ter_clearing, 50, 8);
	generate_biome(m, ter_grass, ter_clearing, 45, 6);
	generate_biome(m, ter_clearing, ter_grass, 35, 4);

	// Generate Trees around mountains
	generate_foothill_forests(m);
	
	// Scatter Boulders and Trees
	if (!scatter_terrain_elements(m)) {
		return -1;
	}

	if (!generate_special_terrain(m)) {
		return -1;
	}

	return 0;
}

int map_generate(map *m, int x, int y, int n, int s, int e, int w)
{
	map_generate_terrain(m);
	map_generate_borders(m);
	map_generate_paths(m, n, s, e, w);
	map_generate_pokeshops(m, x, y);

	return 0;
}
