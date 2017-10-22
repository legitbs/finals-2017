#ifndef CHANNEL_H
#define CHANNEL_H

#include <stdlib.h>
#include <string.h>
#ifdef __clemency__
#include "limits.h"
#else
#include <limits.h>
#endif

#define CHANNEL_NAME_SIZE 32

struct channel {
    char *name;
    unsigned char volume;
    unsigned char muted;
};

void channel_initialize(struct channel *, unsigned int);
void channel_set_name(struct channel *, char *);
char *channel_get_name(struct channel *);
void channel_set_volume(struct channel *, unsigned int);
unsigned int channel_get_volume(struct channel *);
void channel_mute(struct channel *);
void channel_unmute(struct channel *);
unsigned int channel_is_muted(struct channel *);

#endif
