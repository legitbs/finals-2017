#include "vector.h"

void vector_construct(struct vector *vector)
{
    vector->size = 0;
    vector->capacity = VECTOR_DEFAULT_CAPACITY;
    vector->data = malloc(vector->capacity * sizeof(void *));
}

static void vector_resize(struct vector *vector, unsigned int grow)
{
    void *new_data;

    if (grow) {
        vector->capacity += VECTOR_DEFAULT_CAPACITY;
    } else {
        vector->capacity -= VECTOR_DEFAULT_CAPACITY;
    }
    
    if (vector->data) {
        new_data = malloc(vector->capacity * sizeof(void *));
        memcpy(new_data, vector->data, vector->capacity * sizeof(void *));
        free(vector->data);
    } else {
        new_data = malloc(vector->capacity * sizeof(void *));
    }

    vector->data = new_data;
}

void vector_destruct(struct vector *vector)
{
    if (vector->data) {
        free(vector->data);
    }
    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
}

void vector_add(struct vector *vector, void *data)
{
    if (vector->size == vector->capacity) {
        vector_resize(vector, 1);
    }
    vector->data[vector->size++] = data;
}

void vector_remove(struct vector *vector, unsigned int index)
{
    int i;

    if (index < vector->size) {
        for (i = index + 1; i < vector->size; i++) {
            vector->data[i-1] = vector->data[i];
        }
        vector->data[vector->size] = NULL;
        vector->size--;

        if (vector->capacity > VECTOR_DEFAULT_CAPACITY
            && vector->size < vector->capacity - 2 * VECTOR_DEFAULT_CAPACITY)
        {
            vector_resize(vector, 0);
        }
    }
}

void *vector_get(struct vector *vector, unsigned int index)
{
    return vector->data[index];
}

void vector_set(struct vector *vector, unsigned int index, void *data)
{
    if (index < vector->size) {
        vector->data[index] = data;
    }
}

unsigned int vector_size(struct vector *vector)
{
    return vector->size;
}
