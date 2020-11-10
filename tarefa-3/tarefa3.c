#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>

typedef enum TIPO_GRAFO
{
    DIRECIONADO,
    NAO_DIRECIONADO,
} TIPO_GRAFO;

typedef struct no no;
struct no
{
    int num;
    double d;
    double peso;
    no *prox;
};

typedef struct lista
{
    no *cabeca;
    int tam;
} lista;

typedef struct grafo
{
    lista **adj;
    int *vertices;
    double *d;
    int V;
    int E;
    TIPO_GRAFO tipo;
} grafo;

//OPERAÇÕES COM LISTAS DE VÉRTICES
no *new_no(int num, double peso)
{
    if(num >= 0 && peso >= 0)
    {
        no *n = (no*)malloc(sizeof(no));
        if(n != NULL)
        {
            n->peso = peso;
            n->num = num;
            n->prox = NULL;
            return n;
        }
    }
    return NULL;
}

void free_no(no *n)
{
    if(n != NULL)
    {
        free(n);
        n = NULL;
    }
}

lista *new_lista()
{
    lista *l = (lista*)malloc(sizeof(lista));
    if(l != NULL)
    {
        l->cabeca = NULL;
        l->tam = 0;
        return l;
    }
    return NULL;
}

void free_lista(lista *l)
{
    if(l != NULL)
    {
        no *aux = l->cabeca;
        while(aux != NULL)
        {
            l->cabeca = aux->prox;
            free_no(aux);
            aux = l->cabeca;
        }
        free(l);
        l = NULL;
    }
}

void inserir_lista(lista *l, int v, double peso)
{
    if(l != NULL && v >= 0)
    {
        no *n = new_no(v, peso);
        if(n != NULL)
        {
            if(l->cabeca != NULL)
                n->prox = l->cabeca;
            l->cabeca = n;
            l->tam++;
        }
    }
}

int remover_lista(lista *l, int v)
{
    if(l != NULL)
    {
        int rem;
        no *aux = l->cabeca;
        no *ant = NULL;
        while(aux != NULL && aux->num != v)
        {
            ant = aux;
            aux = aux->prox;
        }
        if(aux != NULL)
        {
            if(ant == NULL)
                if(aux->prox == NULL)
                    l->cabeca = NULL;
                else
                    l->cabeca = l->cabeca->prox;
            else
                ant->prox = aux->prox;
            rem = aux->num;
            free_no(aux);
            l->tam--;
            return rem;
        }
    }
    return -1;
}

int buscar_lista(lista *l, int v)
{
    if(l != NULL && l->tam > 0 && v >= 0)
    {
        no *aux = l->cabeca;
        while(aux != NULL && aux->num != v)
            aux = aux->prox;
        if(aux != NULL)
            return 1;
    }
    return 0;
}

//OPERAÇÕES COM GRAFO
void inserir_vertice(grafo *G)
{
    if(G != NULL)
    {
        G->adj = (lista**)realloc(G->adj, (G->V + 1) * sizeof(lista*));
        if(G->adj != NULL)
        {
            lista *l = new_lista();
            G->adj[G->V] = l;
            G->vertices = (int*)realloc(G->vertices, (G->V + 1) * sizeof(int));
            G->vertices[G->V] = G->V;
            G->d = (double*)realloc(G->d, (G->V + 1) * sizeof(double));
            G->V++;
        }
    }
}

void inserir_aresta(grafo *G, int u, int v, double peso)
{
    if(G != NULL && u >= 0 && v >= 0 && u < G->V && v < G->V)
    {
        if(buscar_lista(G->adj[u], v) == 0)
        {
            inserir_lista(G->adj[u], v, peso);
            if(G->tipo == NAO_DIRECIONADO)
                inserir_lista(G->adj[v], u, peso);
            G->E++;
        }
    }
}

grafo *new_grafo(TIPO_GRAFO tipo)
{
    grafo *G = (grafo*)malloc(sizeof(grafo));
    if(G != NULL)
    {
        G->adj = NULL;
        G->vertices = NULL;
        G->d = NULL;
        G->V = 0;
        G->E = 0;
        G->tipo = tipo;
        return G;
    }
    return NULL;
}

void free_grafo(grafo *G)
{
    if(G != NULL)
    {
        for(int i = 0; i < G->V; i++)
            free_lista(G->adj[i]);
        free(G->adj);
        free(G->vertices);
        free(G->d);
        free(G);
    }
}

//ALGORITMOS
grafo *ler_entrada()
{
    int n_vertices;
    scanf("dl\nformat=edgelist1\nn=%d\ndata:\n", &n_vertices);
    grafo *G = new_grafo(DIRECIONADO);
    if(n_vertices > 0 && G != NULL)
    {
        for(int i = 0; i < n_vertices; i++)
            inserir_vertice(G);
        int vert1, vert2;
        double peso;
        while(scanf("%d %d %lf", &vert1, &vert2, &peso) == 3)
        {
            if(vert1 > n_vertices || vert2 > n_vertices)
            {
                perror("Vertice invalido\n");
                if(G != NULL)
                    free_grafo(G);
                G = NULL;
                break;
            }
            inserir_aresta(G, vert1 - 1, vert2 - 1, peso);
        }
        return G;
    }
    else if(G != NULL)
        free_grafo(G);
    return NULL;
} 

void min_heapify(grafo *G, int i, int tam)
{
    int esq = 2 * i;
    int dir = (2 * i) + 1;
    int menor;
    if(esq <= tam && G->d[G->vertices[esq]] < G->d[G->vertices[i]])
        menor = esq;
    else
        menor = i;
    if(dir <= tam && G->d[G->vertices[dir]] < G->d[G->vertices[menor]])
        menor = dir;
    if(menor != i)
    {
        int aux = G->vertices[i];
        G->vertices[i] = G->vertices[menor];
        G->vertices[menor] = aux;
        min_heapify(G, menor, tam);
    }
}

void build_min_heap(grafo *G)
{
    if(G != NULL)
        for(int i = (int)((G->V - 1) / 2); i >= 0; i--)
            min_heapify(G, i, G->V - 1);
}

int extract_min(grafo *G)
{
    int tam = G->V - 1;
    int min = G->vertices[0];
    G->vertices[0] = G->vertices[tam];
    min_heapify(G, 0, tam - 1);
    return min;
}

void decrease_priority(grafo *G, int i, double d)
{
    G->d[i] = d;
    while(i > 0 && G->d[G->vertices[(int)(i / 2)]] > G->d[G->vertices[i]])
    {
        int aux = G->vertices[i];
        G->vertices[i] = G->vertices[(int)(i / 2)];
        G->vertices[(int)(i / 2)] = aux;
        i = (int)(i / 2);
    }
}

void dijkstra(grafo *G, int o)
{
    for(int i = 0; i < G->V; i++)
        G->d[i] = INFINITY;
    G->d[o] = 0;
    build_min_heap(G);
    int i = 0;
    while(i < G->V)
    {
        int u = extract_min(G);
        no *aux = G->adj[u]->cabeca;
        while(aux != NULL)
        {
            if(G->d[aux->num] > G->d[u] + aux->peso)
                decrease_priority(G, aux->num, G->d[u] + aux->peso);
            aux = aux->prox;
        }
        build_min_heap(G);
        i++;
    }
}

int main(void)
{
    grafo *G = ler_entrada();
    if(G != NULL)
    {
        dijkstra(G, 0);
        for(int i = 0; i < G->V; i++)
        {
            if(G->d[i] == INFINITY)
                printf("%d INFINITO\n", i + 1);
            else
                printf("%d %.3lf\n", i + 1, G->d[i]);
        }
        free_grafo(G);
    }
    return 0;
}