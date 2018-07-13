#include <stdlib.h>
#include "linked_list.h"
#include "adjacency_list.h"

struct adjacency_list
{
    linked_list** array;
    int size; 
};

adjacency_list *new_adjacency_list(int size)
{
    if(size >= 0)
    {
        adjacency_list *adj = (adjacency_list*)malloc(sizeof(adjacency_list));
        if(adj != NULL)
        {
            adj->array = (linked_list**)calloc(size, sizeof(linked_list*));
            adj->size = size;
            if(adj->array != NULL)
                return adj;
        }
    }
    return NULL;
}

void free_adjacency_list(adjacency_list *adj)
{
    if(adj != NULL)
    {
        for(int i = 0; i < adj->size; i++)
            free_linked_list(adj->array[i]);
        free(adj);
        adj = NULL;
    }
}

void add_neighbor(adjacency_list *adj, void *neighbor, int index)
{
    if(adj != NULL && index >= 0 && index < adj->size)
        add_linked_list(adj->array[index], neighbor);
}

linked_list *get_neighborhood(adjacency_list *adj, int index)
{
    if(adj != NULL && index >= 0 && index < adj->size)
        return adj->array[index];
    return NULL;
}