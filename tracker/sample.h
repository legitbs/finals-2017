#ifndef SAMPLE_H
#define SAMPLE_H

#include <stdlib.h>
#include <string.h>
#ifdef __clemency__
#include "limits.h"
#else
#include <limits.h>
#endif

#define SAMPLE_NAME_SIZE 30
#define SAMPLE_MAX_DATA_SIZE USHRT_MAX

struct sample {
    unsigned int id;
    char name[SAMPLE_NAME_SIZE];
    unsigned int size;
    unsigned char *data;
    unsigned char references;
};

int sample_get_id(struct sample *);
char *sample_get_name(struct sample *);
void sample_set_name(struct sample *, char *);
unsigned char *sample_get_data(struct sample *, unsigned int *);
void sample_set_data(struct sample *, unsigned int, unsigned char *);
void sample_add_reference(struct sample *);
void sample_remove_reference(struct sample *);

#endif
