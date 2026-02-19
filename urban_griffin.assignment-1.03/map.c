#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "map.h"

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
	for (i = 0; i < h; i++) {
		m->cells[i] = malloc(w * sizeof(terrain_type_t));
		for (j = 0; j < w; j++) {
			m->cells[i][j] = ter_debug;
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
	}
	free(m->cells);

	return 0;
}


/*
* Print the map to the console
*/
int map_print(map *m)
{
	int i, j;
	
	for (i = 0; i < m->height; i++) {
		for (j = 0; j < m->width; j++) {
			printf("%c", map_get_terrain_char(m->cells[i][j])); // Updated to use terrain_type_t
		}
		printf("\n");
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

void dykstra_path(map *m, int start_x, int start_y, int end_x, int end_y) {
	// Placeholder for actual algorithm, draws straight line. Should not draw paths on borders.

	// place a random point between start and end
	int mid_x = (start_x + end_x) / 2 + (rand() % 5) - 2;
	int mid_y = (start_y + end_y) / 2 + (rand() % 5) - 2;
	
	if (mid_x < 1) mid_x = 1;
	if (mid_x > m->width - 2) mid_x = m->width - 2;
	if (mid_y < 1) mid_y = 1;
	if (mid_y > m->height - 2) mid_y = m->height - 2;


	// Draw from start mid, then mid to end
	int x, y;
	x = start_x;
	y = start_y;
	while (x != mid_x || y != mid_y) {
		m->cells[y][x] = ter_path;
		if (x < mid_x && x < m->width - 1) x++;
		else if (x > mid_x && x > 0) x--;
		if (y < mid_y && y < m->height - 1) y++;
		else if (y > mid_y && y > 0) y--;
	}

	x = mid_x;
	y = mid_y;
	while (x != end_x || y != end_y) {
		m->cells[y][x] = ter_path;
		if (x < end_x && x < m->width - 1) x++;
		else if (x > end_x && x > 0) x--;
		if (y < end_y && y < m->height - 1) y++;
		else if (y > end_y && y > 0) y--;
	}


	

}

/*
* Generate NS and EW paths, set 4 border cells to '#', excluding corners, randomly.
* Then, use dykstra's to create paths between them, following edges between terrain types
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
	if (n != -2) dykstra_path(m, north_x, 0, mid_x, mid_y);
	if (s != -2) dykstra_path(m, south_x, m->height - 1, mid_x, mid_y);
	if (e != -2) dykstra_path(m, m->width - 1, east_y, mid_x, mid_y);
	if (w != -2) dykstra_path(m, 0, west_y, mid_x, mid_y);

	// Fix to make sure center isnt overridden
	m->cells[mid_y][mid_x] = ter_path;
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
        	while (!placed) {
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
        	}
    	}

    	//Place PokeMart if chosen
    	if (has_mart) {
        	int placed = 0;
       		while (!placed) {
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
	generate_biome(m, ter_water, ter_clearing, 35, 10);
	generate_biome(m, ter_mountain, ter_clearing, 40, 10);
	generate_biome(m, ter_forest, ter_clearing, 40, 8);
	generate_biome(m, ter_grass, ter_clearing, 55, 8);

	// Foothill forests
	for (y = 0; y < m->height; y++) {
		for (x = 0; x < m->width; x++) {
			if (m->cells[y][x] == ter_clearing) {
				int next_to_mountain = 0;
				for (int ny = y - 1; ny <= y + 1; ny++) {
					for (int nx = x - 1; nx <= x + 1; nx++) {
						if (nx >= 0 && nx < m->width && ny >= 0 && ny < m->height) {
							if (m->cells[ny][nx] == ter_boulder) next_to_mountain = 1;
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

int map_generate(map *m, int x, int y, int n, int s, int e, int w)
{
	map_generate_terrain(m);
	map_generate_borders(m);
	map_generate_paths(m, n, s, e, w);
	map_generate_pokeshops(m, x, y);
	
	/* // Debug
	printf("Gate locations:\n");
	printf("North Gate: (%d, %d)\n", m->north_gate[0], m->north_gate[1]);
	printf("South Gate: (%d, %d)\n", m->south_gate[0], m->south_gate[1]);
	printf("East Gate: (%d, %d)\n", m->east_gate[0], m->east_gate[1]);
	printf("West Gate: (%d, %d)\n", m->west_gate[0], m->west_gate[1]);
	*/

	return 0;
}
