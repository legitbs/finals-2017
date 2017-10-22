#include "list.h"

void list_initialize(struct list *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void list_node_initialize(struct list_node *node, void *data)
{
    if (node != NULL) {
        node->data = data;
        node->prev = NULL;
        node->next = NULL;
    }
}

void list_insert_before(struct list *list, struct list_node *node, struct list_node *new)
{
    if (list != NULL && node != NULL & new != NULL) {
        new->next = node;
        if (node->prev == NULL) {
            new->prev = NULL;
            list->head = new;
        } else {
            new->prev = node->prev;
            node->prev->next = new;
        }
        node->prev = new;
        list->size++;
    }
}

void list_insert_after(struct list *list, struct list_node *node, struct list_node *new)
{
    if (list != NULL && node != NULL && new != NULL) {
        new->prev = node;
        if (node->next == NULL) {
            new->next = NULL;
            list->tail = new;
        } else {
            new->next = node->next;
            node->next->prev = new;
        }
        node->next = new;
        list->size++;
    }
}

void list_insert_head(struct list *list, struct list_node *new)
{
    if (list != NULL && new != NULL) {
        if (list->head == NULL) {
            list->head = new;
            list->tail = new;
            new->prev = NULL;
            new->next = NULL;
            list->size++;
        } else {
            list_insert_before(list, list->head, new);
        }
    }
}

void list_insert_tail(struct list *list, struct list_node *new)
{
    if (list != NULL && new != NULL) {
        if (list->tail == NULL) {
            list_insert_head(list, new);
        } else {
            list_insert_after(list, list->tail, new);
        }
    }
}

void list_remove(struct list *list, struct list_node *node)
{
    if (list != NULL && node != NULL) {
        if (node->prev == NULL) {
            list->head = node->next;
        } else {
            node->prev->next = node->next;
        }

        if (node->next == NULL) {
            list->tail = node->prev;
        } else {
            node->next->prev = node->prev;
        }

        list->size--;
    }
}

struct list_node *list_get_head(struct list *list)
{
    if (list != NULL) {
        return list->head;
    } else {
        return NULL;
    }
}

struct list_node *list_get_tail(struct list *list)
{
    if (list != NULL) {
        return list->tail;
    } else {
        return NULL;
    }
}

int list_size(struct list *list)
{
    if (list != NULL) {
        return list->size;
    } else {
        return -1;
    }
}

int list_empty(struct list *list)
{
    if (list != NULL) {
        return list->size == 0;
    } else {
        return -1;
    }
}
