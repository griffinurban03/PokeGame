#include <stdlib.h>

#include "heap.h"
#include "character.h"

void heap_init(heap_t *h, int capacity) {
	h->capacity = capacity > 0 ? capacity : 128;
	h->size = 0;
	h->data = malloc(h->capacity * sizeof(heap_node_t));
}

void heap_destroy(heap_t *h) {
	free(h->data);
}

bool heap_is_empty(heap_t *h) {
	return h->size == 0;
}

static void swap(heap_node_t *a, heap_node_t *b) {
	heap_node_t temp = *a;
	*a = *b;
	*b = temp;
}

/*
* Tie breaker function, prioitizes sequence numbers if distances are equal
*/
static int compare_nodes(heap_node_t *a, heap_node_t *b) {
	if (a->distance != b->distance) {
		return a->distance - b->distance;
	} 

	return a->seq_num - b->seq_num; // tie-breaker for equal distances
}

void heap_push(heap_t *h, int x, int y, int distance) {
	// Dynamically resize if we run out of room
	if (h->size >= h->capacity) {
		h->capacity *= 2;
		h->data = realloc(h->data, h->capacity * sizeof(heap_node_t));
	}
	
	int i = h->size;
	h->data[i].x = x;
	h->data[i].y = y;
	h->data[i].distance = distance;
	h->size++;

	// Heapify Up
	while (i != 0 && h->data[(i - 1) / 2].distance > h->data[i].distance) {
		swap(&h->data[i], &h->data[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

/*
* Pushes characters onto the heap with a time value
*/
void heap_push_character(heap_t *h, character_t *c, int time, int seq_num) {
	if (h->size >= h->capacity) {
		h->capacity *= 2;
		h->data = realloc(h->data, h->capacity * sizeof(heap_node_t));
	}
	
	int i = h->size;
	h->data[i].x = c->x;
	h->data[i].y = c->y;
	h->data[i].distance = time; // distance holds time (cost)
	h->data[i].c = c;
	h->data[i].seq_num = seq_num;
	h->size++;

	// Heapify Up using compare_nodes
	while (i != 0 && compare_nodes(&h->data[(i - 1) / 2], &h->data[i]) > 0) {
		swap(&h->data[i], &h->data[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

bool heap_pop(heap_t *h, heap_node_t *out_node) {
	if (h->size <= 0) return false;
	
	if (out_node) *out_node = h->data[0];
	
	h->size--;
	if (h->size == 0) return true;
	
	h->data[0] = h->data[h->size];
	
	// Heapify Down using compare_nodes
	int i = 0;
	while (1) {
		int left = 2 * i + 1;
		int right = 2 * i + 2;
		int smallest = i;

		if (left < h->size && compare_nodes(&h->data[left], &h->data[smallest]) < 0) {
			smallest = left;
		}
		if (right < h->size && compare_nodes(&h->data[right], &h->data[smallest]) < 0) {
			smallest = right;
		}

		if (smallest != i) {
			swap(&h->data[i], &h->data[smallest]);
			i = smallest;
		} else {
			break;
		}
	}
	return true;
}