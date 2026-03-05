#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>

typedef struct heap_node {
	int x;
	int y;
	int distance;
	struct character *c;
	int seq_num;
} heap_node_t;

typedef struct heap {
	heap_node_t *data;
	int size;
	int capacity;
} heap_t;

void heap_init(heap_t *h, int capacity);
void heap_destroy(heap_t *h);
void heap_push(heap_t *h, int x, int y, int distance);
void heap_push_character(heap_t *h, struct character *c, int time, int seq_num);
bool heap_pop(heap_t *h, heap_node_t *out_node);
bool heap_is_empty(heap_t *h);

#endif