#include "linked_list.h"

typedef struct adjacency_list adjacency_list;

adjacency_list *new_adjacency_list(int size);

void free_adjacency_list(adjacency_list *adj);

void add_neighbor(adjacency_list *adj, void *neighbor, int index);

linked_list *get_neighborhood(adjacency_list *adj, int index);