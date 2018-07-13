typedef struct linked_list linked_list;

typedef struct iterator iterator;

linked_list *new_linked_list();

void free_linked_list(linked_list *lkdlst);

void add_linked_list(linked_list *lkdlst, void *key);

iterator *get_iterator(linked_list *lkdlst);

void *get_next_iterator(iterator *it);