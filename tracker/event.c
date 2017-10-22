#include "event.h"

struct event *event_create(unsigned int note, unsigned int octave, unsigned int volume,
                           unsigned int instrument, unsigned int effect, unsigned int parameter)
{
    struct event *event = malloc(sizeof(struct event));

    event->note = note;
    event->octave = octave;
    event->volume = volume;
    event->instrument = instrument;
    event->effect = effect;
    event->parameter = parameter;

    return event;
}

void event_destroy(struct event *event)
{
    if (event) {
        free(event);
    }
}
