#ifndef TRACKER_H
#define TRACKER_H

#include <stdlib.h>
#ifdef __clemency__
#include "limits.h"
#include "debug.h"
#else
#include <limits.h>
#endif
#include "channel.h"
#include "pattern.h"
#include "event.h"
#include "instrument.h"
#include "sample.h"
#include "list.h"
#include "vector.h"

#define TRACKER_SUCCESS 0
#define TRACKER_FAILURE -1
#define TRACKER_MAX_FRAMES (CHAR_MAX + 1)
#define TRACKER_MAX_CHANNELS 8
#define TRACKER_MAX_PATTERNS (UCHAR_MAX + 1)
#define TRACKER_MAX_PATTERN_LENGTH 128
#define TRACKER_MAX_INSTRUMENTS (CHAR_MAX + 1)
#define TRACKER_MAX_SAMPLES (UCHAR_MAX + 1)
#define TRACKER_MAX_SEMITONES 12
#define TRACKER_MAX_OCTAVES 8
#define TRACKER_MAX_VOLUME 64
#define TRACKER_DEFAULT_VOLUME 64
#define TRACKER_MAX_TEMPO 300
#define TRACKER_DEFAULT_TEMPO 150
#define TRACKER_MAX_SPEED 32
#define TRACKER_DEFAULT_SPEED 16

struct tracker {
    struct vector frames;
    struct channel channels[TRACKER_MAX_CHANNELS];
    struct list patterns;
    unsigned int next_pattern_id;
    unsigned char pattern_length;
    struct vector instruments;
    unsigned int next_instrument_id;
    int last_free_instrument_id;
    struct vector samples;
    unsigned int next_sample_id;
    int last_free_sample_id;
    unsigned char initial_volume;
    unsigned char initial_tempo;
    unsigned char initial_speed;
    unsigned char current_volume;
    unsigned char current_tempo;
    unsigned char current_speed;
};

void tracker_initialize(struct tracker *);

// state
unsigned int tracker_get_initial_volume(struct tracker *);
void tracker_set_initial_volume(struct tracker *, unsigned int);
unsigned int tracker_get_initial_tempo(struct tracker *);
void tracker_set_initial_tempo(struct tracker *, unsigned int);
unsigned int tracker_get_initial_speed(struct tracker *);
void tracker_set_initial_speed(struct tracker *, unsigned int);
unsigned int tracker_get_current_volume(struct tracker *);
void tracker_set_current_volume(struct tracker *, unsigned int);
unsigned int tracker_get_current_tempo(struct tracker *);
void tracker_set_current_tempo(struct tracker *, unsigned int);
unsigned int tracker_get_current_speed(struct tracker *);
void tracker_set_current_speed(struct tracker *, unsigned int);
unsigned int tracker_get_pattern_length(struct tracker *);
void tracker_set_pattern_length(struct tracker *, unsigned int);
unsigned int tracker_get_next_pattern_id(struct tracker *);
unsigned int tracker_get_next_instrument_id(struct tracker *);
unsigned int tracker_get_next_sample_id(struct tracker *);

// frames
int tracker_add_frame(struct tracker *, unsigned char *);
int tracker_remove_frame(struct tracker *, unsigned int);
unsigned char *tracker_get_frame(struct tracker *, unsigned int);
int tracker_set_frame(struct tracker *, unsigned int, unsigned char *);
unsigned int tracker_get_total_frames(struct tracker *);

// channels
char *tracker_get_channel_name(struct tracker *, unsigned int);
int tracker_set_channel_name(struct tracker *, unsigned int, unsigned char *);
int tracker_get_channel_volume(struct tracker *, unsigned int);
int tracker_set_channel_volume(struct tracker *, unsigned int, unsigned int);
int tracker_mute_channel(struct tracker *, unsigned int);
int tracker_unmute_channel(struct tracker *, unsigned int);

// patterns
int tracker_create_pattern(struct tracker *);
struct pattern *tracker_get_pattern(struct tracker *, unsigned int);
void tracker_destroy_pattern(struct tracker *, unsigned int);
void tracker_update_pattern_lengths(struct tracker *);
int tracker_add_event_to_pattern(struct tracker *, unsigned int, unsigned int,
                                 unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
struct event *tracker_get_event_from_pattern(struct tracker *, unsigned int, unsigned int);
void tracker_remove_event_from_pattern(struct tracker *, unsigned int, unsigned int);

// instruments
int tracker_add_instrument(struct tracker *, char *, unsigned int);
struct instrument *tracker_get_instrument(struct tracker *, unsigned int);
char *tracker_get_instrument_name(struct tracker *, unsigned int);
void tracker_set_instrument_name(struct tracker *, unsigned int, char *);
struct sample *tracker_get_instrument_sample(struct tracker *, unsigned int);
void tracker_set_instrument_sample(struct tracker *, unsigned int, unsigned int);
unsigned int tracker_get_instrument_volume(struct tracker *, unsigned int);
void tracker_set_instrument_volume(struct tracker *, unsigned int, unsigned int);

// samples
int tracker_add_sample(struct tracker *, char *, unsigned int, unsigned char *);
struct sample *tracker_get_sample(struct tracker *, unsigned int);
char *tracker_get_sample_name(struct tracker *, unsigned int);
void tracker_set_sample_name(struct tracker *, unsigned int, char *);
unsigned char *tracker_get_sample_data(struct tracker *, unsigned int, unsigned int *);
void tracker_set_sample_data(struct tracker *, unsigned int, unsigned int, unsigned char *);
void tracker_remove_sample(struct tracker *, unsigned int);

#endif
