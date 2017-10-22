#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <stdlib.h>
#include "sample.h"

#define INSTRUMENT_MAX_VOLUME ((UCHAR_MAX + 1) / (CHAR_BIT))
#define INSTRUMENT_NAME_SIZE 32

struct instrument {
    unsigned char id;
    char *name;
    struct sample *sample;
    unsigned char volume;
    unsigned char loop;
    unsigned int loop_point;
};

int instrument_get_id(struct instrument *);
char *instrument_get_name(struct instrument *);
void instrument_set_name(struct instrument *, char *);
struct sample *instrument_get_sample(struct instrument *);
void instrument_set_sample(struct instrument *, struct sample *);
unsigned int instrument_get_volume(struct instrument *);
void instrument_set_volume(struct instrument *, unsigned int );
int instrument_get_loop(struct instrument *);
void instrument_set_loop(struct instrument *, unsigned int, unsigned int);

#endif
