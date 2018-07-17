#include <stdlib.h>
#include "adjacency_list.h"
#include "graph.h"

struct vertex
{
    unsigned int number;
    vertex_decoration *decoration;
};

struct edge
{
    unsigned int number;
    vertex *source;
    vertex *destination;
    edge_decoration *decoration;
};

enum graph_type
{
    DIRECTED,
    UNDIRECTED,
};

struct graph
{
    adjacency_list *adj;
    unsigned int number_V;
    unsigned int number_E;
    unsigned int limit_V;
    unsigned int limit_E;
    vertex **V;
    edge **E;
    graph_type type;
};

edge *new_edge(unsigned int number, vertex *source, vertex *destination, edge_decoration *decoration)
{
    if(source != NULL && destination != NULL)
    {
        edge *e = (edge*)malloc(sizeof(edge));
        if(e != NULL)
        {
            e->number = number;
            e->source = source;
            e->destination = destination;
            e->decoration = decoration;
            return e;
        }
    }
    return NULL;
}

void free_edge(edge *e)
{
    if(e != NULL)
    {
        free(e);
        e = NULL;
    }
}

vertex *new_vertex(unsigned int number, vertex_decoration *decoration)
{
    vertex *v = (vertex*)malloc(sizeof(vertex));
    if(v != NULL)
    {
        v->number = number;
        v->decoration = decoration;
        return v;
    }
    return NULL;
}

void free_vertex(vertex *v)
{
    if(v != NULL)
    {
        free(v);
        v = NULL;
    }
}

graph *new_graph(graph_type type)
{
    graph *G = (graph*)malloc(sizeof(graph));
    if(G != NULL)
    {
        G->adj = NULL;
        G->number_V = 0;
        G->number_E = 0;
        G->limit_V = 0;
        G->limit_E = 0;
        G->V = NULL;
        G->E = NULL;
        G->type = type;
        return G;
    }
    return NULL;
}

void free_graph(graph *G)
{
    if(G != NULL)
    {
        if(G->adj != NULL)
            free_adjacency_list(G->adj);
        if(G->V != NULL)
        {
            for(int i = 0; i < G->number_V; i++)
                free_vertex(G->V[i]);
            free(G->V);
        }
        if(G->E != NULL)
        {
            for(int i = 0; i < G->number_E; i++)
                free_edge(G->E[i]);
            free(G->E);
        } 
        free(G);
        G = NULL;
    }
}

void add_vertex(graph *G, vertex_decoration *decoration)
{
    if(G != NULL)
    {

    }
}