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

int map_init(map *m, int w, int h)
{
	m->width = w;
	m->height = h;

	// Create the 2D array for the map cells of % for testing
	m->cells = malloc(h * sizeof(char *));
	for (int i = 0; i < h; i++) {
		m->cells[i] = malloc(w * sizeof(char));
		for (int j = 0; j < w; j++) {
			m->cells[i][j] = '0';
		}
	}

	return 0;
}

int map_destroy(map *m)
{
	// Free the allocated memory for the map cells

	return 0;
}

int map_print(map *m)
{
	// Print the map to the console
	for (int i = 0; i < m->height; i++) {
		for (int j = 0; j < m->width; j++) {
			printf("%c", m->cells[i][j]);
		}
		printf("\n");
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
		int x = rand() % m->width;
		int y = rand() % m->height;
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
	
	// place % around the border of the map
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
