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
int generate_borders(map *m) {
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

/*
* Generate a random map with different terrain types 
*/
int map_generate(map *m)
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

	if(!generate_borders(m)) {
		return -1;
	}
	
	return 0;
}
