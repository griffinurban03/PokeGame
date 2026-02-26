#include "heap.h"
#include <stdlib.h>

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

bool heap_pop(heap_t *h, heap_node_t *out_node) {
	if (h->size <= 0) return false;
	
	// Output the node with the lowest distance
	if (out_node) *out_node = h->data[0];
	
	h->size--;
	if (h->size == 0) return true;
	
	// Move the last element to the root
	h->data[0] = h->data[h->size];
	
	// Heapify Down
	int i = 0;
	while (1) {
		int left = 2 * i + 1;
		int right = 2 * i + 2;
		int smallest = i;

		if (left < h->size && h->data[left].distance < h->data[smallest].distance) {
			smallest = left;
		}
		if (right < h->size && h->data[right].distance < h->data[smallest].distance) {
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