#include "tracker.h"

void tracker_initialize(struct tracker *tracker)
{
    int i;

    // initialize global state
    tracker->next_pattern_id = 0;
    tracker->next_instrument_id = 0;
    tracker->last_free_instrument_id = -1;
    tracker->next_sample_id = 0;
    tracker->last_free_sample_id = -1;
    tracker->pattern_length = TRACKER_MAX_PATTERN_LENGTH / 2;
    tracker->initial_volume = TRACKER_DEFAULT_VOLUME;
    tracker->initial_tempo = TRACKER_DEFAULT_TEMPO;
    tracker->initial_speed = TRACKER_DEFAULT_SPEED;

    // construct and initialize frame vector
    unsigned char *frame = malloc(TRACKER_MAX_CHANNELS * sizeof(unsigned char));
    for (i = 0; i < TRACKER_MAX_CHANNELS; i++) {
        frame[i] = 0;
    }
    vector_construct(&tracker->frames);
    tracker_add_frame(tracker, frame);

    // initialize channels
    for (i = 0; i < TRACKER_MAX_CHANNELS; i++) {
        channel_initialize(&tracker->channels[i], TRACKER_MAX_VOLUME);
    }

    // initialize patterns list
    list_initialize(&tracker->patterns);

    // construct instrument and sample vectors
    vector_construct(&tracker->instruments);
    vector_construct(&tracker->samples);
}


/* STATE */

unsigned int tracker_get_initial_speed(struct tracker *tracker)
{
    return tracker->initial_speed;
}

void tracker_set_initial_speed(struct tracker *tracker, unsigned int speed)
{
    tracker->initial_speed = (speed > TRACKER_MAX_SPEED) ? TRACKER_MAX_SPEED : speed;
}

unsigned int tracker_get_initial_tempo(struct tracker *tracker)
{
    return tracker->initial_tempo;
}

void tracker_set_initial_tempo(struct tracker *tracker, unsigned int tempo)
{
    tracker->initial_tempo = (tempo > TRACKER_MAX_TEMPO) ? TRACKER_MAX_TEMPO : tempo;
}

unsigned int tracker_get_initial_volume(struct tracker *tracker)
{
    return tracker->initial_volume;
}

void tracker_set_initial_volume(struct tracker *tracker, unsigned int volume)
{
    tracker->initial_volume = (volume > TRACKER_MAX_VOLUME) ? TRACKER_MAX_VOLUME : volume;
}

unsigned int tracker_get_current_speed(struct tracker *tracker)
{
    return tracker->current_speed;
}

void tracker_set_current_speed(struct tracker *tracker, unsigned int speed)
{
    tracker->current_speed = (speed > TRACKER_MAX_SPEED) ? TRACKER_MAX_SPEED : speed;
}

unsigned int tracker_get_current_tempo(struct tracker *tracker)
{
    return tracker->current_tempo;
}

void tracker_set_current_tempo(struct tracker *tracker, unsigned int tempo)
{
    tracker->current_tempo = (tempo > TRACKER_MAX_TEMPO) ? TRACKER_MAX_TEMPO : tempo;
}

unsigned int tracker_get_current_volume(struct tracker *tracker)
{
    return tracker->current_volume;
}

void tracker_set_current_volume(struct tracker *tracker, unsigned int volume)
{
    tracker->current_volume = (volume > TRACKER_MAX_VOLUME) ? TRACKER_MAX_VOLUME : volume;
}

unsigned int tracker_get_pattern_length(struct tracker *tracker)
{
    return tracker->pattern_length;
}

void tracker_set_pattern_length(struct tracker *tracker, unsigned int length)
{
    tracker->pattern_length = length > TRACKER_MAX_PATTERN_LENGTH ? TRACKER_MAX_PATTERN_LENGTH : length;
    tracker_update_pattern_lengths(tracker);
}

unsigned int tracker_get_next_pattern_id(struct tracker *tracker)
{
    return tracker->next_pattern_id++;
}

unsigned int tracker_get_next_instrument_id(struct tracker *tracker)
{
    int id;

    if (tracker->last_free_instrument_id >= 0) {
        id = tracker->last_free_instrument_id;

        tracker->last_free_instrument_id = tracker->next_instrument_id;
        do {
            struct instrument *instrument = tracker_get_instrument(tracker, tracker->last_free_instrument_id);
            if (!instrument && tracker->last_free_instrument_id == tracker->next_instrument_id) {
                break;
            } else if (!instrument) {
                return id;
            }
        } while (++tracker->last_free_instrument_id != tracker->next_instrument_id);
        tracker->last_free_instrument_id = -1;
        return id;
    } else {
        id = tracker->next_instrument_id++;
        if (tracker->next_instrument_id >= TRACKER_MAX_INSTRUMENTS) {
            tracker->next_instrument_id = 0;
        }
        return id;
    }
}

unsigned int tracker_get_next_sample_id(struct tracker *tracker)
{
    int id;

    if (tracker->last_free_sample_id >= 0) {
        id = tracker->last_free_sample_id;

        tracker->last_free_sample_id = tracker->next_sample_id;
        do {
            struct sample *sample = tracker_get_sample(tracker, tracker->last_free_sample_id);
            if (!sample && tracker->last_free_sample_id == tracker->next_sample_id) {
                break;
            } else if (!sample) {
                return id;
            }
        } while (++tracker->last_free_sample_id != tracker->next_sample_id);
        tracker->last_free_sample_id = -1;
        return id;
    } else {
        id = tracker->next_sample_id++;
        if (tracker->next_sample_id >= TRACKER_MAX_SAMPLES) {
            tracker->next_sample_id = 0;
        }
        return id;
    }
}


/* FRAMES */
int tracker_add_frame(struct tracker *tracker, unsigned char *frame)
{
    if (vector_size(&tracker->frames) < TRACKER_MAX_FRAMES) {
        vector_add(&tracker->frames, frame);
        return TRACKER_SUCCESS;
    } else {
        return TRACKER_FAILURE;
    }
}

int tracker_remove_frame(struct tracker *tracker, unsigned int index)
{
    unsigned char *frame;

    if (index < vector_size(&tracker->frames)) {
        frame = vector_get(&tracker->frames, index);
        vector_remove(&tracker->frames, index);

        if (frame) {
            free(frame);
        }

        return TRACKER_SUCCESS;
    } else {
        return TRACKER_FAILURE;
    }
}

unsigned char *tracker_get_frame(struct tracker *tracker, unsigned int index)
{
    return (unsigned char *)vector_get(&tracker->frames, index);
}

int tracker_set_frame(struct tracker *tracker, unsigned int index, unsigned char *frame)
{
    if (index < vector_size(&tracker->frames)) {
        vector_set(&tracker->frames, index, frame);
        return TRACKER_SUCCESS;
    } else {
        return TRACKER_FAILURE;
    }
}

unsigned int tracker_get_total_frames(struct tracker *tracker)
{
    return vector_size(&tracker->frames);
}


/* CHANNELS */
char *tracker_get_channel_name(struct tracker *tracker, unsigned int index)
{
    if (index < TRACKER_MAX_CHANNELS) {
        return channel_get_name(&tracker->channels[index]);
    } else {
        return NULL;
    }
}

int tracker_set_channel_name(struct tracker *tracker, unsigned int index, unsigned char *name)
{
    if (index < TRACKER_MAX_CHANNELS) {
        channel_set_name(&tracker->channels[index], name);
        return TRACKER_SUCCESS;
    } else {
        return TRACKER_FAILURE;
    }
}

int tracker_get_channel_volume(struct tracker *tracker, unsigned int index)
{
    if (index < TRACKER_MAX_CHANNELS) {
        return channel_get_volume(&tracker->channels[index]);
    } else {
        return TRACKER_FAILURE;
    }
}

int tracker_set_channel_volume(struct tracker *tracker, unsigned int index, unsigned int volume)
{
    if (index < TRACKER_MAX_CHANNELS) {
        channel_set_volume(&tracker->channels[index], volume);
        return TRACKER_SUCCESS;
    } else {
        return TRACKER_FAILURE;
    }
}

int tracker_mute_channel(struct tracker *tracker, unsigned int index)
{
    if (index < TRACKER_MAX_CHANNELS) {
        channel_mute(&tracker->channels[index]);
        return TRACKER_SUCCESS;
    } else {
        return TRACKER_FAILURE;
    }
}

int tracker_unmute_channel(struct tracker *tracker, unsigned int index)
{
    if (index < TRACKER_MAX_CHANNELS) {
        channel_unmute(&tracker->channels[index]);
        return TRACKER_SUCCESS;
    } else {
        return TRACKER_FAILURE;
    }
}


/* PATTERNS */

int tracker_create_pattern(struct tracker *tracker)
{
    struct list_node *entry = malloc(sizeof(struct list_node));

    if (list_size(&tracker->patterns) <= TRACKER_MAX_PATTERNS) {
        list_node_initialize(entry, pattern_create());
        pattern_set_id((struct pattern *)entry->data, tracker_get_next_pattern_id(tracker));
        pattern_resize((struct pattern *)entry->data, tracker->pattern_length);
        list_insert_tail(&tracker->patterns, entry);
        return pattern_get_id((struct pattern *)entry->data);
    } else {
        return TRACKER_FAILURE;
    }
}

struct pattern *tracker_get_pattern(struct tracker *tracker, unsigned int id)
{
    struct list_node *entry = list_get_head(&tracker->patterns);
    struct pattern *pattern = NULL;

    while (entry) {
        pattern = (struct pattern *)entry->data;
        if (pattern && pattern->id == id) {
            break;
        }
        pattern = NULL;
        entry = entry->next;
    }

    return pattern;
}

void tracker_destroy_pattern(struct tracker *tracker, unsigned int id)
{
    pattern_destroy(tracker_get_pattern(tracker, id));
}

void tracker_update_pattern_lengths(struct tracker *tracker)
{
    struct list_node *entry = list_get_head(&tracker->patterns);

    while (entry) {
        pattern_resize((struct pattern *)entry->data, tracker->pattern_length);
        entry = entry->next;
    }
}

int tracker_add_event_to_pattern(struct tracker *tracker, unsigned int pat_id, unsigned int evt_idx,
                                  unsigned int note, unsigned int octave, unsigned int volume,
                                  unsigned int instrument, unsigned int effect, unsigned int parameter)
{
    if (evt_idx < tracker->pattern_length) {
        struct event *event = event_create(note, octave, volume, instrument, effect, parameter);
        pattern_add_event(tracker_get_pattern(tracker, pat_id), evt_idx, event);
        return TRACKER_SUCCESS;
    } else {
        return TRACKER_FAILURE;
    }
}

struct event *tracker_get_event_from_pattern(struct tracker *tracker, unsigned int pat_id, unsigned int evt_idx)
{
    struct pattern *pattern = tracker_get_pattern(tracker, pat_id);
    if (pattern == NULL) {
        return NULL;
    }

    return pattern_get_event(pattern, evt_idx);
}

void tracker_remove_event_from_pattern(struct tracker *tracker, unsigned int pat_id, unsigned int evt_idx)
{
    pattern_remove_event(tracker_get_pattern(tracker, pat_id), evt_idx);
}


/* INSTRUMENTS */

int tracker_add_instrument(struct tracker *tracker, char *name, unsigned int smp_id)
{
    struct instrument *instrument;

    if (vector_size(&tracker->instruments) >= TRACKER_MAX_INSTRUMENTS) {
        return TRACKER_FAILURE;
    }

    instrument = malloc(sizeof(struct instrument));
    instrument->id = tracker_get_next_instrument_id(tracker);
    instrument->name = name;
    instrument->sample = tracker_get_sample(tracker, smp_id);
    instrument->volume = TRACKER_MAX_VOLUME;

    vector_add(&tracker->instruments, instrument);
    return instrument->id;
}

struct instrument *tracker_get_instrument(struct tracker *tracker, unsigned int id)
{
    struct instrument *instrument = NULL;
    int i;

    for (i = 0; i < vector_size(&tracker->instruments); i++) {
        instrument = (struct instrument *)vector_get(&tracker->instruments, i);
        if (instrument->id == id) {
            break;
        }
        instrument = NULL;
    }

    return instrument;
}

char *tracker_get_instrument_name(struct tracker *tracker, unsigned int id)
{
    instrument_get_name(tracker_get_instrument(tracker, id));
}

void tracker_set_instrument_name(struct tracker *tracker, unsigned int id, char *name)
{
    instrument_set_name(tracker_get_instrument(tracker, id), name);
}

struct sample *tracker_get_instrument_sample(struct tracker *tracker, unsigned int id)
{
    instrument_get_sample(tracker_get_instrument(tracker, id));
}

void tracker_set_instrument_sample(struct tracker *tracker, unsigned int ins_id, unsigned int smp_id)
{
    instrument_set_sample(tracker_get_instrument(tracker, ins_id), tracker_get_sample(tracker, smp_id));
}

unsigned int tracker_get_instrument_volume(struct tracker *tracker, unsigned int id)
{
    return instrument_get_volume(tracker_get_instrument(tracker, id));
}

void tracker_set_instrument_volume(struct tracker *tracker, unsigned int id, unsigned int volume)
{
    instrument_set_volume(tracker_get_instrument(tracker, id), volume);
}


/* SAMPLES */

int tracker_add_sample(struct tracker *tracker, char *name, unsigned int size, unsigned char *data)
{
    struct sample *sample = malloc(sizeof(struct sample));

    if (vector_size(&tracker->samples) >= TRACKER_MAX_SAMPLES) {
        return TRACKER_FAILURE;
    }

    sample = malloc(sizeof(struct sample));
    sample->id = tracker_get_next_sample_id(tracker);
    memcpy(&sample->name, name, SAMPLE_NAME_SIZE);
    sample->size = size;
    sample->data = data;
    sample->references = 0;

    vector_add(&tracker->samples, sample);
    return sample->id; 
}

struct sample *tracker_get_sample(struct tracker *tracker, unsigned int id)
{
    struct sample *sample;
    int i;

    for (i = 0; i < vector_size(&tracker->samples); i++) {
        sample = (struct sample *)vector_get(&tracker->samples, i);
        if (sample->id == id) {
            break;
        }
        sample = NULL;
    }

    return sample;
}

char *tracker_get_sample_name(struct tracker *tracker, unsigned int id)
{
    return sample_get_name(tracker_get_sample(tracker, id));
}

void tracker_set_sample_name(struct tracker *tracker, unsigned int id, char *name)
{
    sample_set_name(tracker_get_sample(tracker, id), name);
}

unsigned char *tracker_get_sample_data(struct tracker *tracker, unsigned int id, unsigned int *size)
{
    return sample_get_data(tracker_get_sample(tracker, id), size);
}

void tracker_set_sample_data(struct tracker *tracker, unsigned int id, unsigned int size, unsigned char *data)
{
    sample_set_data(tracker_get_sample(tracker, id), size, data);
}

void tracker_remove_sample(struct tracker *tracker, unsigned int id)
{
    struct sample *sample;
    struct instrument *instrument;
    int i;
    int j;

    for (i = 0; i < vector_size(&tracker->samples); i++) {
        sample = (struct sample *)vector_get(&tracker->samples, i);
        if (sample->id == id) {
            // if the matching sample still has references to it, remove them
            if (sample->references) {
                for (j = 0; j < vector_size(&tracker->instruments); j++) {
                    instrument = (struct instrument *)vector_get(&tracker->instruments, j);
                    if (instrument->sample && instrument->sample->id == sample->id) {
                        instrument->sample == NULL;
                        sample_remove_reference(sample);
                        if (!sample->references) {
                            break;
                        }
                    }
                }
            }
            free(sample->data);
            vector_remove(&tracker->samples, i);
            break;
        }
    }
    tracker->last_free_sample_id = id;
}
