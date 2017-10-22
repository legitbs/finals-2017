#ifndef EVENT_H
#define EVENT_H

#include <stdlib.h>

struct event {
    unsigned char note;
    unsigned char octave;
    unsigned char volume;
    unsigned char instrument;
    unsigned char effect;
    unsigned char parameter;
};

struct event *event_create(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
void event_destroy(struct event *);

#endif
