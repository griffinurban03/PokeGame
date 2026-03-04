#ifndef PATH_H
#define PATH_H

#include "map.h"
#include "character.h"

// Calc the distance for hikers and rivals
void pathfind_build_distance_map(map *m, character_t *pc);

// prints distance maps in debug format (also for assignment 1.03 hehe)
void pathfind_print_distance_map(map *m, character_type_t npc_type);

#endif