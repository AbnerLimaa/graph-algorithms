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

typedef struct grafo
{
    lista **adj;
    int V;
    int E;
    TIPO_GRAFO tipo;
} grafo;

typedef struct DFS
{
    COR *cor;
    int *pai;
    int *tempo_inicial;
    int *tempo_final;
    int tempo;
} DFS;

static int *componentes_conexas;
static int componente_atual = 0;

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

void inserir_aresta(grafo *G, int u, int v)
{
    if(G != NULL && u >= 0 && v >= 0 && u < G->V && v < G->V)
    {
        if(buscar_lista(G->adj[u], v) == 0)
        {
            inserir_lista(G->adj[u], v);
            if(G->tipo == NAO_DIRECIONADO)
                inserir_lista(G->adj[v], u);
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
        free(G);
    }
}

//ALGORITMOS
DFS *new_dfs(int tamanho)
{
    if(tamanho > 0)
    {
        DFS *dfs = (DFS*)malloc(sizeof(DFS));
        if(dfs != NULL)
        {
            dfs->cor = (COR*)malloc(tamanho * sizeof(COR));
            dfs->pai = (int*)malloc(tamanho * sizeof(int));
            dfs->tempo_inicial = (int*)malloc(tamanho * sizeof(int));
            dfs->tempo_final = (int*)malloc(tamanho * sizeof(int));
            dfs->tempo = 0;
            return dfs;
        }
    }
    return NULL;
}

void free_dfs(DFS *dfs)
{
    if(dfs != NULL)
    {
        free(dfs->cor);
        free(dfs->pai);
        free(dfs->tempo_inicial);
        free(dfs->tempo_final);
        free(dfs);
    }
}

void dfs_visitar(grafo *G, DFS *dfs, int u)
{
    dfs->tempo++;
    dfs->tempo_inicial[u] = dfs->tempo;
    dfs->cor[u] = CINZA;
    no *aux = G->adj[u]->cabeca;
    while(aux != NULL)
    {
        if(dfs->cor[aux->num] == BRANCO)
        {
            dfs->pai[aux->num] = u;
            dfs_visitar(G, dfs, aux->num);
        }
        aux = aux->prox;
    }
    dfs->cor[u] = PRETO;
    dfs->tempo++;
    dfs->tempo_final[u] = dfs->tempo;
    componentes_conexas[u] = componente_atual;
}

void busca_profundidade(grafo *G)
{
    DFS *dfs = new_dfs(G->V);
    if(G != NULL && G->V > 0 && dfs != NULL)
    {
        for(int i = 0; i < G->V; i++)
        {
            dfs->cor[i] = BRANCO;
            dfs->pai[i] = -1;
        }
        for(int i = 0; i < G->V; i++)
            if(dfs->cor[i] == BRANCO)
            {
                dfs_visitar(G, dfs, i);
                componente_atual++;
            }
    }
    free_dfs(dfs);
}

grafo *ler_entrada()
{
    int n_vertices;
    scanf("dl\nformat=edgelist1\nn=%d\ndata:\n", &n_vertices);
    grafo *G = new_grafo(NAO_DIRECIONADO);
    if(n_vertices > 0 && G != NULL)
    {
        componentes_conexas = (int*)malloc(n_vertices * sizeof(int));
        for(int i = 0; i < n_vertices; i++)
            inserir_vertice(G);
        int vert1, vert2;
        while(scanf("%d %d", &vert1, &vert2) == 2)
        {
            if(vert1 > n_vertices || vert2 > n_vertices)
            {
                perror("Vertice invalido\n");
                if(G != NULL)
                    free_grafo(G);
                G = NULL;
                break;
            }
            inserir_aresta(G, vert1 - 1, vert2 - 1);
        }
        return G;
    }
    else if(G != NULL)
        free_grafo(G);
    return NULL;
} 

void imprimir_componentes(grafo *G)
{
    if(G != NULL)
    {
        for(int i = 0; i < componente_atual; i++)
        {
            for(int j = 0; j < G->V; j++)
            {
                if(componentes_conexas[j] == i)
                    printf("%d ", j + 1);
            }
            printf("\n");
        }
        free(componentes_conexas);
    }
}

int main(void)
{
    grafo *G = ler_entrada();
    if(G != NULL)
    {
        busca_profundidade(G);
        imprimir_componentes(G);
        free_grafo(G); 
    }
    return 0;
}