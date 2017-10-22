#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

struct list {
    struct list_node *head;
    struct list_node *tail;
    unsigned int size;
};

struct list_node {
    void *data;
    struct list_node *next;
    struct list_node *prev;
};

void list_initialize(struct list *);
void list_node_initialize(struct list_node *, void *);
void list_insert_before(struct list *, struct list_node *, struct list_node *);
void list_insert_after(struct list *, struct list_node *, struct list_node *);
void list_insert_head(struct list *, struct list_node *);
void list_insert_tail(struct list *, struct list_node *);
void list_remove(struct list *, struct list_node *);
struct list_node *list_get_head(struct list *);
struct list_node *list_get_tail(struct list *);
int list_size(struct list *);
int list_empty(struct list *);

#endif
