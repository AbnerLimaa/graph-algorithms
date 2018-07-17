#include <stdlib.h>
#include "linked_list.h"

typedef struct node node;

struct node
{
    void *key;
    node *nxt;
};

struct iterator
{
    node *nd;
    void *key;
};

struct linked_list
{
    node *head;
};

node *new_node(void *key)
{
    if(key != NULL)
    {
        node *nd = (node*)malloc(sizeof(node));
        if(nd == NULL)
            return NULL;
        nd->key = key;
        nd->nxt = NULL;
        return nd;
    }
    return NULL;
}

void free_node(node *nd)
{
    if(nd != NULL)
        free(nd);
}

iterator *new_iterator(node *nd)
{
    iterator *it = (iterator*)malloc(sizeof(iterator));
    if(it == NULL)
        return NULL;
    it->nd = nd;
    it->key = NULL;
    return it;
}

void free_iterator(iterator *it)
{
    if(it != NULL)
        free(it);
}

linked_list *new_linked_list()
{
    linked_list *lkdlst = (linked_list*)malloc(sizeof(linked_list));
    if(lkdlst == NULL)
        return NULL;
    lkdlst->head = NULL;
    return lkdlst;
}

void free_linked_list(linked_list *lkdlst)
{
    if(lkdlst != NULL)
    {
        node *nd = lkdlst->head;
        node *aux;
        while(nd != NULL)
        {
            aux = nd->nxt;
            free_node(nd);
            nd = aux;
        }
        free(lkdlst);
        lkdlst = NULL;
    }
}

void add_linked_list(linked_list *lkdlst, void *key)
{
    if(lkdlst != NULL && key != NULL)
    {
        node *nd = new_node(key);
        if(nd != NULL)
        {
            if(lkdlst->head == NULL)
                lkdlst->head = nd;
            else
            {
                nd->nxt = lkdlst->head;
                lkdlst->head = nd;
            }
        }
    }
}

iterator *get_iterator(linked_list *lkdlst)
{
    if(lkdlst != NULL && lkdlst->head != NULL)
    {
        iterator *it = new_iterator(lkdlst->head);
        return it;
    }
    return NULL;
}

void *get_next_iterator(iterator *it)
{
    if(it != NULL && it->nd != NULL)
    {
        it->key = it->nd->key;
        it->nd = it->nd->nxt;
        return it->key;
    }
    return NULL;
}