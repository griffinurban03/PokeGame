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
int map_generate_paths(map *m) {

	int north_x = (rand() % (m->width - 4)) + 3;
	int south_x = (rand() % (m->width - 4)) + 3;
	int east_y = (rand() % (m->height - 4)) + 3;
	int west_y = (rand() % (m->height - 4)) + 3;

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
int map_generate_pokeshops(map *m) {
	int p = 0;
	while(p < 2) {
		int x = rand() % (m->width - 4) + 2;
		int y = rand() % (m->height - 4) + 2;

		// Check if adjacent to path
		if ((m->cells[y-1][x] == '#'|| m->cells[y+2][x] == '#') && 
			(m->cells[y][x+1] != '#') && 
			(m->cells[y+1][x] != '#') && 
			(m->cells[y+1][x+1] != '#')) 
		{
			if (p == 0) {
				m->cells[y][x] = 'M';
				m->cells[y][x+1] = 'M';
				m->cells[y+1][x] = 'M';
				m->cells[y+1][x+1] = 'M';
				p++;
			} else {
				m->cells[y][x] = 'C';
				m->cells[y][x+1] = 'C';
				m->cells[y+1][x] = 'C';
				m->cells[y+1][x+1] = 'C';
				p++;
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
	
	return 0;
}

int map_generate(map *m)
{
	map_generate_terrain(m);
	map_generate_borders(m);
	map_generate_paths(m);
	map_generate_pokeshops(m);


	// DEBUG
	printf("Gate locations:\n");
	printf("North Gate: (%d, %d)\n", m->north_gate[0], m->north_gate[1]);
	printf("South Gate: (%d, %d)\n", m->south_gate[0], m->south_gate[1]);
	printf("East Gate: (%d, %d)\n", m->east_gate[0], m->east_gate[1]);
	printf("West Gate: (%d, %d)\n", m->west_gate[0], m->west_gate[1]);
	
	return 0;
}
