#include "map.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

enum terrain_chars {
	TERRAIN_SHORT_GRASS = '.',
	TERRAIN_TALL_GRASS = ':',
	TERRAIN_BOULDER = '%',
	TERRAIN_TREE = '^',
	TERRAIN_WATER = '~'
};

char terrain_type[] = {
	TERRAIN_SHORT_GRASS,
	TERRAIN_TALL_GRASS,
	TERRAIN_BOULDER,
	TERRAIN_TREE,
	TERRAIN_WATER
};

enum terrain_t {
	ter_debug,
	ter_boulder,
	ter_tree,
	ter_path,
	ter_mart,
	ter_center,
	ter_grass,
	ter_clearing,
	ter_mountain,
	ter_forest,
	ter_water,
	ter_gate
} terrain_type_t;

/*
* Initialize the map with given width and height. Creates a 2D array for the map cells with the '0' character.
*/
int map_init(map *m, int w, int h)
{
	int i, j;

	m->width = w;
	m->height = h;

	m->cells = malloc(h * sizeof(char *));
	for (i = 0; i < h; i++) {
		m->cells[i] = malloc(w * sizeof(char));
		for (j = 0; j < w; j++) {
			m->cells[i][j] = '0';
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
			printf("%c", m->cells[i][j]);
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
		m->cells[0][x] = '%';
		m->cells[m->height - 1][x] = '%';
	}
	for (y = 0; y < m->height; y++) {
		m->cells[y][0] = '%';
		m->cells[y][m->width - 1] = '%';
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
		m->cells[y][x] = '#';
		if (x < mid_x && x < m->width - 1) x++;
		else if (x > mid_x && x > 0) x--;
		if (y < mid_y && y < m->height - 1) y++;
		else if (y > mid_y && y > 0) y--;
	}

	x = mid_x;
	y = mid_y;
	while (x != end_x || y != end_y) {
		m->cells[y][x] = '#';
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

	m->cells[0][north_x] = '#';
	m->cells[m->height - 1][south_x] = '#';
	m->cells[east_y][0] = '#';
	m->cells[west_y][m->width - 1] = '#';

	m->north_gate[0] = north_x;
	m->north_gate[1] = 0;
	m->south_gate[0] = south_x;
	m->south_gate[1] = m->height - 1;
	m->east_gate[0] = 0;
	m->east_gate[1] = east_y;
	m->west_gate[0] = m->width - 1;
	m->west_gate[1] = west_y;

	// connect north to south
	dykstra_path(m, north_x, 0, south_x, m->height - 1);

	// connect east to west
	dykstra_path(m, 0, east_y, m->width - 1, west_y);

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
            		if ((m->cells[my-1][mx] == '#'|| m->cells[my+2][mx] == '#' ||
                		 m->cells[my][mx-1] == '#'|| m->cells[my][mx+2] == '#') && 
                		(m->cells[my][mx] != 'M' && m->cells[my][mx] != 'C') &&
				(m->cells[my][mx] != '#') &&
                		(m->cells[my][mx+1] != '#') && 
                		(m->cells[my+1][mx] != '#') && 
                		(m->cells[my+1][mx+1] != '#')) 
            		{
                		m->cells[my][mx] = 'C';
                		m->cells[my][mx+1] = 'C';
                		m->cells[my+1][mx] = 'C';
                		m->cells[my+1][mx+1] = 'C';
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

            		if ((m->cells[my-1][mx] == '#'|| m->cells[my+2][mx] == '#' ||
                 		m->cells[my][mx-1] == '#'|| m->cells[my][mx+2] == '#') && 
                		(m->cells[my][mx] != 'C' && m->cells[my][mx] != 'M') &&
                		(m->cells[my][mx] != '#') &&
				(m->cells[my][mx+1] != '#') && 
                		(m->cells[my+1][mx] != '#') && 
                		(m->cells[my+1][mx+1] != '#')) 
            		{
                		m->cells[my][mx] = 'M';
                		m->cells[my][mx+1] = 'M';
                		m->cells[my+1][mx] = 'M';
                		m->cells[my+1][mx+1] = 'M';
                		placed = 1;
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
	int i;
	int x, y;
	static int32_t offsets[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};

	// Seed Random Number Generator
	srand(time(NULL));

	// Place 8 terrain "seeds" across the map, then grow them out to create clusters of terrain
	for (i = 0; i < 20; i++) {
		x = rand() % m->width;
		y = rand() % m->height;
		m->cells[y][x] = terrain_type[(i % 5)];
	}

	// Grow cells until the map is filled
	// Use multiple passes, each pass randomly selects cells to grow
	int iterations = 0;
	int cells_filled = 1;
	
	while (cells_filled > 0 && iterations < 1000) {
		cells_filled = 0;
		// Iterate random order to avoid "stalagtite phenomenon" i.e. top left to bottom right growth patterns (i can explain i know this sounds insane)
		for (i = 0; i < m->width * m->height; i++) {
			x = rand() % m->width;
			y = rand() % m->height;
			
			if (m->cells[y][x] != '0') {
				// Try to grow into one random neighbor
				int random_offset = rand() % 8;
				int nx = x + offsets[random_offset][0];
				int ny = y + offsets[random_offset][1];
				
				if (nx >= 0 && nx < m->width && ny >= 0 && ny < m->height && m->cells[ny][nx] == '0') {
					m->cells[ny][nx] = m->cells[y][x];
					cells_filled++;
				}
			}
		}
		iterations++;
	}

	// final pass to fill any remaining empty cells with short grass
	for (y = 0; y < m->height; y++) {
		for (x = 0; x < m->width; x++) {
			if (m->cells[y][x] == '0') {
				m->cells[y][x] = TERRAIN_SHORT_GRASS;
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
