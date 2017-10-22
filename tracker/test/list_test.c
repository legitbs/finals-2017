#include <stdio.h>
#include "list.h"
#ifdef __clemency__
#include "assert.h"
#else
#include <assert.h>
#endif

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    puts("Starting list test...");
    fflush(stdout);

    // create test values
    struct list list;
    list_initialize(&list);
    struct list_node *node1 = malloc(sizeof(struct list_node));
    struct list_node *node2 = malloc(sizeof(struct list_node));
    struct list_node *node3 = malloc(sizeof(struct list_node));
    struct list_node *node4 = malloc(sizeof(struct list_node));
    struct list_node *node5 = malloc(sizeof(struct list_node));
    int one = 1;
    int two = 2;
    int three = 3;
    int four = 4;
    int five = 5;
    list_node_initialize(node1, &one);
    list_node_initialize(node2, &two);
    list_node_initialize(node3, &three);
    list_node_initialize(node4, &four);
    list_node_initialize(node5, &five);

    // test list_empty, list_size
    assert(list_size(&list) == 0);
    assert(list_empty(&list));

    // test list_insert_head, list_insert_tail, list_get_head, list_get_tail
    list_insert_tail(&list, node5);
    list_insert_head(&list, node1);
    assert(list_get_head(&list) == node1);
    assert(list_get_tail(&list) == node5);
    assert(list_size(&list) == 2);
    assert(!list_empty(&list));

    // test list_insert_before, list_insert_after
    list_insert_after(&list, node1, node2);
    list_insert_before(&list, node5, node4);
    list_insert_after(&list, node2, node3);
    assert(list_get_head(&list) == node1);
    assert(list_get_tail(&list) == node5);
    assert(node1->prev == NULL);
    assert(node1->next == node2);
    assert(node2->prev == node1);
    assert(node2->next == node3);
    assert(node3->prev == node2);
    assert(node3->next == node4);
    assert(node4->prev == node3);
    assert(node4->next == node5);
    assert(node5->prev == node4);
    assert(node5->next == NULL);
    assert(list_size(&list) == 5);
    assert(!list_empty(&list));

    // test list_remove
    list_remove(&list, node3);
    assert(node2->next == node4);
    assert(node4->prev == node2);
    //assert(node3->next == NULL);
    //assert(node3->prev == NULL);
    assert(list_size(&list) == 4);
    assert(!list_empty(&list));
    list_remove(&list, node1);
    list_remove(&list, node5);
    assert(list_get_head(&list) == node2);
    assert(node2->prev == NULL);
    assert(node2->next == node4);
    assert(node4->prev == node2);
    assert(node4->next == NULL);
    assert(list_get_tail(&list) == node4);
    assert(list_size(&list) == 2);
    assert(!list_empty(&list));
    list_remove(&list, node2);
    list_remove(&list, node4);
    assert(list_size(&list) == 0);
    assert(list_empty(&list));
    
    free(node1);
    free(node2);
    free(node3);
    free(node4);
    free(node5);
    puts("List test has concluded");
    fflush(stdout);

    return 0;
}
