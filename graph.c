#include <stdlib.h>
#include "graph.h"

struct vertex
{
    unsigned int position;
    vertex_decoration *decoration;
};

struct edge
{
    unsigned int position;
    unsigned int position_neighborhood;
    vertex *source;
    vertex *destination;
    edge_decoration *decoration;
};

struct neighborhood
{
    vertex *home;
    edge **neighbors;
    unsigned int number_E;
    unsigned int limit_E;
};

enum graph_type
{
    DIRECTED,
    UNDIRECTED,
};

struct graph
{
    neighborhood **adj;
    unsigned int number_V;
    unsigned int limit_V;
    unsigned int number_E;
    unsigned int limit_E;
    vertex **V;
    edge **E;
    graph_type type;
};

vertex *new_vertex(unsigned int position, vertex_decoration *decoration)
{
    vertex *v = (vertex*)malloc(sizeof(vertex));
    if(v != NULL)
    {
        v->position = position;
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

edge *new_edge(unsigned int position, unsigned int position_neighborhood, vertex *source, vertex *destination, edge_decoration *decoration)
{
    if(source != NULL && destination != NULL)
    {
        edge *e = (edge*)malloc(sizeof(edge));
        if(e != NULL)
        {
            e->position = position;
            e->position_neighborhood = position_neighborhood;
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

neighborhood *new_neighborhood(vertex *home)
{
    if(home != NULL)
    {
        neighborhood *n = (neighborhood*)malloc(sizeof(neighborhood));
        if(n != NULL)
        {
            n->home = home;
            n->neighbors = NULL;
            n->number_E = 0;
            n->limit_E = 0;
            return n;
        }
    }
    return NULL;
}

void free_neighborhood(neighborhood *n)
{
    if(n != NULL)
    {
        if(n->neighbors != NULL)
            free(n->neighbors);
        free(n);
        n = NULL;
    }
}

graph *new_graph(graph_type type)
{
    graph *G = (graph*)malloc(sizeof(graph));
    if(G != NULL)
    {
        G->adj = NULL;
        G->number_V = 0;
        G->limit_V = 0;
        G->number_E = 0;
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
        {
            for(int i = 0; i < G->limit_V; i++)
                free_neighborhood(G->adj[i]);
            free(G->adj);
        }
        if(G->V != NULL)
        {
            for(int i = 0; i < G->limit_V; i++)
                free_vertex(G->V[i]);
            free(G->V);
        }
        if(G->E != NULL)
        {
            for(int i = 0; i < G->limit_E; i++)
                free_edge(G->E[i]);
            free(G->E);
        } 
        free(G);
        G = NULL;
    }
}

vertex *add_vertex(graph *G, vertex_decoration *decoration)
{
    if(G != NULL)
    {
        vertex *new_v = new_vertex(G->number_V, decoration);
        neighborhood *new_vertex_neighborhood = new_neighborhood(new_v);
        if(new_v != NULL && new_vertex_neighborhood != NULL)
        {
            if(G->number_V == G->limit_V)
            {
                unsigned int new_limit_V = (G->limit_V + 1) << 1;
                vertex **new_vertex_array = (vertex**)calloc(new_limit_V, sizeof(vertex*));
                neighborhood **new_adj = (neighborhood**)calloc(new_limit_V, sizeof(neighborhood*));
                if(new_vertex_array != NULL && new_adj != NULL)
                {
                    G->limit_V = new_limit_V;
                    for(int i = 0; i < G->number_V; i++)
                    {
                        new_vertex_array[i] = G->V[i];
                        new_adj[i] = G->adj[i];
                    }
                    new_vertex_array[G->number_V] = new_v;
                    new_adj[G->number_V] = new_vertex_neighborhood;
                    G->number_V++;
                    if(G->V != NULL)
                        free(G->V);
                    if(G->adj != NULL)
                        free(G->adj);
                    G->V = new_vertex_array;
                    G->adj = new_adj;
                }
                else
                {
                    free_vertex(new_v);
                    free_neighborhood(new_vertex_neighborhood);
                    if(new_vertex_array != NULL)
                        free(new_vertex_array);
                    if(new_adj != NULL)
                        free(new_adj);
                    return NULL;    
                } 
            }
            else
            {
                G->V[G->number_V] = new_v;
                G->adj[G->number_V] = new_vertex_neighborhood;
                G->number_V++;
            }
            return new_v;
        }
        else
        {
            if(new_v != NULL)
                free_vertex(new_v);
            if(new_vertex_neighborhood != NULL)
                free_neighborhood(new_vertex_neighborhood);
        }
    }
    return NULL;
}

void remove_vertex(graph *G, vertex *v)
{
    if(G != NULL && v != NULL && G->number_V > 0 && v->position < G->number_V && G->V[v->position] == v)
    {
        if(G->number_V <= G->limit_V >> 2)
        {
            unsigned int new_limit_V = G->limit_V >> 1;
            vertex **new_vertex_array = (vertex**)calloc(new_limit_V, sizeof(vertex*));
            neighborhood **new_adj = (neighborhood**)calloc(new_limit_V, sizeof(neighborhood*));
            if(new_vertex_array != NULL && new_adj != NULL)
            {
                for(int i = 0; i < G->limit_V; i++)
                {
                    if(i < G->number_V)
                    {
                        new_vertex_array[i] = G->V[i];
                        new_adj[i] = G->adj[i];
                    }
                    else
                        free_neighborhood(G->adj[i]);
                }
                G->limit_V = new_limit_V;
                free(G->V);
                free(G->adj);
                G->V = new_vertex_array;
                G->adj = new_adj;
            }
            else
            {
                if(new_vertex_array != NULL)
                    free(new_vertex_array);
                if(new_adj != NULL)
                    free(new_adj);
                exit(1);
            }
        }
        vertex *v_aux = G->V[v->position];
        G->V[G->number_V - 1]->position = v->position;
        G->V[v->position] = G->V[G->number_V - 1];
        G->V[G->number_V - 1] = v_aux;

        neighborhood *n_aux = G->adj[v->position];
        G->adj[v->position] = G->adj[G->number_V - 1];
        G->adj[G->number_V - 1] = n_aux;

        free_vertex(v);
        G->V[G->number_V - 1] = NULL;
        G->number_V--;
    }
}

edge *add_edge(graph *G, vertex *source, vertex *destination, edge_decoration *decoration)
{
    if(G != NULL)
    {
        
    }
}

void remove_edge(graph *G, edge *e)
{

}