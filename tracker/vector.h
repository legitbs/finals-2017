#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>

#define VECTOR_DEFAULT_CAPACITY 64

struct vector {
    void **data;
    unsigned int size;
    unsigned int capacity;
};

void vector_construct(struct vector *);
static void vector_resize(struct vector *, unsigned int grow);
void vector_destruct(struct vector *);

void vector_add(struct vector *, void *);
void vector_remove(struct vector *, unsigned int);
void *vector_get(struct vector *, unsigned int);
void vector_set(struct vector *, unsigned int, void *);

unsigned int vector_size(struct vector *);

#endif
