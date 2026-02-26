#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>

typedef struct heap_node {
	int x;
	int y;
	int distance;
} heap_node_t;

typedef struct heap {
	heap_node_t *data;
	int size;
	int capacity;
} heap_t;

void heap_init(heap_t *h, int capacity);
void heap_destroy(heap_t *h);
void heap_push(heap_t *h, int x, int y, int distance);
bool heap_pop(heap_t *h, heap_node_t *out_node);
bool heap_is_empty(heap_t *h);

#endif