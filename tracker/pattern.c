#include "pattern.h"

struct pattern *pattern_create()
{
    struct pattern *pattern = malloc(sizeof(struct pattern));

    pattern->id = 0;
    vector_construct(&pattern->events);

    return pattern;
}

void pattern_resize(struct pattern *pattern, unsigned int size)
{
    int i;

    if (pattern == NULL) {
        return;
    }

    if (size == pattern->events.size) {
        return;
    } else if (size > pattern->events.size) {
        for (i = pattern->events.size; i <= size; i++) {
            vector_add(&pattern->events, NULL);
        }
    } else {
        for (i = pattern->events.size; i == size; i--) {
            event_destroy(vector_get(&pattern->events, i));
            vector_remove(&pattern->events, i);
        }
    }
}

void pattern_destroy(struct pattern *pattern)
{
    int i;

    if (pattern != NULL) {
        for (i = 0; i < pattern->events.size; i++) {
            event_destroy(vector_get(&pattern->events, i));
        }
        free(pattern);
    }
}

void pattern_set_id(struct pattern *pattern, unsigned int id)
{
    if (pattern != NULL) {
        pattern->id = id;
    }
}

int pattern_get_id(struct pattern *pattern)
{
    if (pattern != NULL) {
        return pattern->id;
    } else {
        return -1;
    }
}

void pattern_add_event(struct pattern *pattern, unsigned int index, struct event *event)
{
    if (pattern != NULL) {
        vector_set(&pattern->events, index, event);
    }
}

struct event *pattern_get_event(struct pattern *pattern, unsigned int index)
{
    if (pattern != NULL) {
        return vector_get(&pattern->events, index);
    } else {
        return NULL;
    }
}

void pattern_remove_event(struct pattern *pattern, unsigned int index)
{
    if (pattern != NULL && index < pattern->events.size) {
        event_destroy(vector_get(&pattern->events, index));
        vector_set(&pattern->events, index, NULL);
    }
}
