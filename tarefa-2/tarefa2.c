#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum COR
{
    BRANCO,
    CINZA,
    PRETO,
} COR;

typedef enum TIPO_GRAFO
{
    DIRECIONADO,
    NAO_DIRECIONADO,
} TIPO_GRAFO;

typedef struct no no;
struct no
{
    int num;
    no *prox;
};

typedef struct lista
{
    no *cabeca;
    int tam;
} lista;

typedef struct aresta
{
    int u;
    int v;
    double peso;
} aresta;

typedef struct grafo
{
    lista **adj;
    int V;
    int E;
    aresta **arestas;
    TIPO_GRAFO tipo;
} grafo;

static double peso_AGM = 0;
static int conjunto_atual = 0;
static int *conjuntos = NULL;
static COR *cores = NULL;

//OPERAÇÕES COM LISTAS DE VÉRTICES
no *new_no(int num)
{
    if(num >= 0)
    {
        no *n = (no*)malloc(sizeof(no));
        if(n != NULL)
        {
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

void inserir_lista(lista *l, int v)
{
    if(l != NULL && v >= 0)
    {
        no *n = new_no(v);
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
            G->V++;
        }
    }
}

aresta *new_aresta(int u, int v, double peso)
{
    if(u >= 0 && v >= 0)
    {
        aresta *a = (aresta*)malloc(sizeof(aresta));
        if(a != NULL)
        {
            a->u = u;
            a->v = v;
            a->peso = peso;
            return a;
        }
    }
    return NULL;
}

void free_aresta(aresta *a)
{
    if(a != NULL)
        free(a);
}

void inserir_aresta(grafo *G, int u, int v, double peso)
{
    if(G != NULL && u >= 0 && v >= 0 && u < G->V && v < G->V)
    {
        if(buscar_lista(G->adj[u], v) == 0)
        {
            inserir_lista(G->adj[u], v);
            if(G->tipo == NAO_DIRECIONADO)
                inserir_lista(G->adj[v], u);
            G->E++;
            G->arestas = (aresta**)realloc(G->arestas, G->E * sizeof(aresta*));
            G->arestas[G->E - 1] = new_aresta(u, v, peso);
        }
    }
}

grafo *new_grafo(TIPO_GRAFO tipo)
{
    grafo *G = (grafo*)malloc(sizeof(grafo));
    if(G != NULL)
    {
        G->adj = NULL;
        G->V = 0;
        G->E = 0;
        G->arestas = NULL;
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
        for(int i = 0; i < G->E; i++)
            free_aresta(G->arestas[i]);
        free(G->arestas);
        free(G->adj);
        free(G);

    }
}

//ALGORITMOS
grafo *ler_entrada()
{
    int n_vertices;
    scanf("dl\nformat=edgelist1\nn=%d\ndata:\n", &n_vertices);
    grafo *G = new_grafo(NAO_DIRECIONADO);
    if(n_vertices > 0 && G != NULL)
    {
        conjuntos = (int*)malloc(n_vertices * sizeof(int));
        cores = (COR*)malloc(n_vertices * sizeof(COR));
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

void max_heapify(grafo *G, int i, int tamanho)
{
    int esq = 2 * i;
    int dir = (2 * i) + 1;
    int maior = -1;
    if(esq <= tamanho && G->arestas[esq]->peso > G->arestas[i]->peso)
        maior = esq;
    else
        maior = i;
    if(dir <= tamanho && G->arestas[dir]->peso > G->arestas[maior]->peso)
        maior = dir;
    if(maior != i)
    {
        aresta *aux = G->arestas[i];
        G->arestas[i] = G->arestas[maior];
        G->arestas[maior] = aux;
        max_heapify(G, maior, tamanho);
    }
}

void build_max_heap(grafo *G)
{
    for(int i = (int)((G->E - 1) / 2); i >= 0; i--)
        max_heapify(G, i, G->E - 1);
}

void heapsort(grafo *G)
{
    build_max_heap(G);
    int tamanho = G->E - 1;
    for(int i = G->E - 1; i >= 1; i--)
    {
        aresta *aux = G->arestas[0];
        G->arestas[0] = G->arestas[i];
        G->arestas[i] = aux;
        tamanho--;
        max_heapify(G, 0, tamanho);
    }
}

void iniciar_conjuntos(grafo *G)
{
    if(G != NULL)
    {
        for(int i = 0; i < G->V; i++)
        {
            conjuntos[i] = conjunto_atual;
            conjunto_atual++;
        }
    }
}

void dfs_visitar(grafo *G, int u)
{
    cores[u] = CINZA;
    conjuntos[u] = conjunto_atual;
    no *aux = G->adj[u]->cabeca;
    while(aux != NULL)
    {
        if(cores[aux->num] == BRANCO)
        {
            conjuntos[aux->num] = conjunto_atual;
            dfs_visitar(G, aux->num);
        }
        aux = aux->prox;
    }
    cores[u] = PRETO;
}

void busca_profundidade(grafo *G, int u)
{
    if(G != NULL && G->V > 0 && u >= 0 && u < G->V)
    {
        for(int i = 0; i < G->V; i++)
            cores[i] = BRANCO;
        dfs_visitar(G, u);
    }
}

double kruskal(grafo *G)
{
    grafo *T = new_grafo(NAO_DIRECIONADO);
    for(int i = 0; i < G->V; i++)
        inserir_vertice(T);
    iniciar_conjuntos(G);
    heapsort(G);
    for(int i = 0; i < G->E; i++)
    {
        aresta *a = G->arestas[i];
        if(conjuntos[a->u] != conjuntos[a->v])
        {
            peso_AGM += a->peso;
            busca_profundidade(T, a->u);
            busca_profundidade(T, a->v);
            inserir_aresta(T, a->u, a->v, a->peso);
            conjunto_atual++;
        }
    }
    free_grafo(T);
    return peso_AGM;
}

int main(void)
{
    grafo *G = ler_entrada();
    if(G != NULL)
    {
        printf("%.3lf", kruskal(G));
        free_grafo(G);
        free(conjuntos); 
        free(cores);
    }
    return 0;
}