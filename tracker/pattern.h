#ifndef PATTERN_H
#define PATTERN_H

#include <stdlib.h>
#include "vector.h"
#include "event.h"

struct pattern {
    unsigned int id;
    struct vector events;
};

struct pattern *pattern_create();
void pattern_resize(struct pattern *, unsigned int);
void pattern_destroy(struct pattern *);
void pattern_set_id(struct pattern *, unsigned int);
int pattern_get_id(struct pattern *);
struct event *pattern_get_event(struct pattern *, unsigned int);
void pattern_add_event(struct pattern *, unsigned int, struct event *);
void pattern_remove_event(struct pattern *, unsigned int);

#endif
