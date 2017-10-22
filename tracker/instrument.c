#include "instrument.h"

int instrument_get_id(struct instrument *instrument)
{
    if (instrument) {
        return instrument->id;
    } else {
        return -1;
    }
}

char *instrument_get_name(struct instrument *instrument)
{
    if (instrument) {
        return instrument->name;
    } else {
        return NULL;
    }
}

void instrument_set_name(struct instrument *instrument, char *name)
{
    if (instrument) {
        instrument->name = name;
    }
}

struct sample *instrument_get_sample(struct instrument *instrument)
{
    if (instrument) {
        return instrument->sample;
    } else {
        return NULL;
    }
}

void instrument_set_sample(struct instrument *instrument, struct sample *sample)
{
    if (instrument) {
        if (instrument->sample) {
            sample_remove_reference(instrument->sample);
        }
        instrument->sample = sample;
        sample_add_reference(instrument->sample);
    }
}

unsigned int instrument_get_volume(struct instrument *instrument)
{
    if (instrument) {
        return instrument->volume;
    }
}

void instrument_set_volume(struct instrument *instrument, unsigned int volume)
{
    if (instrument) {
        instrument->volume = volume < INSTRUMENT_MAX_VOLUME ? volume : INSTRUMENT_MAX_VOLUME;
    }
}

int instrument_get_loop(struct instrument *instrument)
{
    if (instrument) {
        if (instrument->loop) {
            return instrument->loop_point;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

void instrument_set_loop(struct instrument *instrument, unsigned int loop, unsigned int loop_point)
{
    if (instrument) {
        if (loop && loop_point < instrument->sample->size) {
            instrument->loop = 1;
            instrument->loop_point = loop_point;
        } else {
            instrument->loop = 0;
            instrument->loop_point = -1;
        }
    }
}
