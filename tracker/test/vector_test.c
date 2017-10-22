#include <stdio.h>
#include "vector.h"
#ifdef __clemency__
#include "assert.h"
#else
#include <assert.h>
#endif

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    puts("Starting vector test...");
    fflush(stdout);
    
    // create test values
    int one = 1;
    int two = 2;
    int three = 3;
    int four = 4;
    int five = 5;
    int six = 6;
    int seven = 7;

    // test vector_construct
    struct vector vector;
    vector_construct(&vector);

    // test vector_add, vector_get, and vector_size
    vector_add(&vector, &one);
    vector_add(&vector, &two);
    vector_add(&vector, &three);
    vector_add(&vector, &four);
    vector_add(&vector, &five);
    assert(*(int *)vector_get(&vector, 0) == 1);
    assert(*(int *)vector_get(&vector, 1) == 2);
    assert(*(int *)vector_get(&vector, 2) == 3);
    assert(*(int *)vector_get(&vector, 3) == 4);
    assert(*(int *)vector_get(&vector, 4) == 5);
    assert(vector_size(&vector) == 5);

    // test vector_remove
    vector_remove(&vector, 1);
    vector_remove(&vector, 2);
    assert(*(int *)vector_get(&vector, 0) == 1);
    assert(*(int *)vector_get(&vector, 1) == 3);
    assert(*(int *)vector_get(&vector, 2) == 5);
    assert(vector_size(&vector) == 3);

    // test vector_set
    vector_set(&vector, 0, &seven);
    vector_set(&vector, 1, &six);
    assert(*(int *)vector_get(&vector, 0) == 7);
    assert(*(int *)vector_get(&vector, 1) == 6);
    assert(*(int *)vector_get(&vector, 2) == 5);
    assert(vector_size(&vector) == 3);

    // test vector_destruct
    vector_destruct(&vector);
    assert(vector.data == NULL);
    assert(vector_size(&vector) == 0);

    // test vector_resize
    vector_add(&vector, &one);
    vector_add(&vector, &two);
    vector_add(&vector, &three);
    assert(*(int *)vector_get(&vector, 0) == 1);
    assert(*(int *)vector_get(&vector, 1) == 2);
    assert(*(int *)vector_get(&vector, 2) == 3);
    assert(vector_size(&vector) == 3);

    vector_destruct(&vector);
    puts("Vector test has concluded");
    fflush(stdout);

    return 0;
}
