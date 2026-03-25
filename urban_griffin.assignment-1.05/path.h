#ifndef PATH_H
#define PATH_H

#include "map.h"
#include "character.h"

struct world;

// Calc the distance for hikers and rivals
void pathfind_build_distance_map(struct world *w, map *m);

// prints distance maps in debug format (also for assignment 1.03 hehe)
void pathfind_print_distance_map(struct world *w, character_type_t type);

#endif