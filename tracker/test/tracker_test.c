#include "tracker.h"
#ifdef __clemency__
#include "assert.h"
#else
#include <assert.h>
#endif

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    // create test variables
    struct tracker tracker;
    struct pattern *pattern;
    struct event *event;
    struct instrument *instrument;
    struct sample *sample;
    tracker_initialize(&tracker);
    char *data;
    char *name;
    int size;
    int id;
    int i;
    int j;

    puts("Starting tracker test...");

    // test getting and setting state
    puts("Starting state tests...");
    assert(tracker_get_initial_volume(&tracker) == TRACKER_DEFAULT_VOLUME);
    assert(tracker_get_initial_tempo(&tracker) == TRACKER_DEFAULT_TEMPO);
    assert(tracker_get_initial_speed(&tracker) == TRACKER_DEFAULT_SPEED);
    assert(tracker_get_pattern_length(&tracker) == TRACKER_MAX_PATTERN_LENGTH/2);
    tracker_set_initial_volume(&tracker, 32);
    tracker_set_initial_tempo(&tracker, 200);
    tracker_set_initial_speed(&tracker, 24);
    assert(tracker_get_initial_volume(&tracker) == 32);
    assert(tracker_get_initial_tempo(&tracker) == 200);
    assert(tracker_get_initial_speed(&tracker) == 24);
    puts("State tests concluded...");

    // test channels
    puts("Starting channel tests...");
    tracker_set_channel_name(&tracker, 0, "ONE");
    tracker_set_channel_name(&tracker, 1, "");
    tracker_set_channel_name(&tracker, 2, "THREETHREETHREE");
    tracker_set_channel_name(&tracker, 3, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    tracker_set_channel_name(&tracker, 4, "5");
    tracker_set_channel_name(&tracker, 5, "SIX");
    tracker_set_channel_name(&tracker, 6, "SEVEN");
    tracker_set_channel_name(&tracker, 7, "EIGHT");
    assert(strcmp(tracker_get_channel_name(&tracker, 0), "ONE") == 0);
    assert(strcmp(tracker_get_channel_name(&tracker, 1), "") == 0);
    assert(strcmp(tracker_get_channel_name(&tracker, 2), "THREETHREETHREE") == 0);
    assert(strcmp(tracker_get_channel_name(&tracker, 3), "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA") == 0);
    assert(strcmp(tracker_get_channel_name(&tracker, 4), "5") == 0);
    assert(strcmp(tracker_get_channel_name(&tracker, 5), "SIX") == 0);
    assert(strcmp(tracker_get_channel_name(&tracker, 6), "SEVEN") == 0);
    assert(strcmp(tracker_get_channel_name(&tracker, 7), "EIGHT") == 0);
    tracker_set_channel_volume(&tracker, 0, 0);
    tracker_set_channel_volume(&tracker, 1, 8);
    tracker_set_channel_volume(&tracker, 2, 16);
    tracker_set_channel_volume(&tracker, 3, 24);
    tracker_set_channel_volume(&tracker, 4, 32);
    tracker_set_channel_volume(&tracker, 5, 40);
    tracker_set_channel_volume(&tracker, 6, 48);
    tracker_set_channel_volume(&tracker, 7, 56);
    assert(tracker_get_channel_volume(&tracker, 0) == 0);
    assert(tracker_get_channel_volume(&tracker, 1) == 8);
    assert(tracker_get_channel_volume(&tracker, 2) == 16);
    assert(tracker_get_channel_volume(&tracker, 3) == 24);
    assert(tracker_get_channel_volume(&tracker, 4) == 32);
    assert(tracker_get_channel_volume(&tracker, 5) == 40);
    assert(tracker_get_channel_volume(&tracker, 6) == 48);
    assert(tracker_get_channel_volume(&tracker, 7) == 56);
    tracker_set_channel_volume(&tracker, 0, 64);
    assert(tracker_get_channel_volume(&tracker, 0) == 64);
    // TODO: test muting/unmuting channels
    puts("Channel tests concluded");


    // test patterns
    puts("Starting pattern test...");
    for (i = 0; i < TRACKER_MAX_PATTERNS; i++) {
        id = tracker_create_pattern(&tracker);
        assert(id != -1);
        pattern = tracker_get_pattern(&tracker, id);
        assert(pattern != NULL);
        tracker_add_event_to_pattern(&tracker, id, 0, 11, 12, 13, 14, 15, 16);
        assert(tracker_get_event_from_pattern(&tracker, id, 0) != NULL);
    }
    puts("Pattern tests concluded");

    // TODO
    tracker_set_pattern_length(&tracker, 32);
    // TODO

    // test instruments
    puts("Starting instrument tests...");
    for (i = 0; i < TRACKER_MAX_INSTRUMENTS; i++) {
        name = malloc(INSTRUMENT_NAME_SIZE * sizeof(char));
        name[0] = 'A';
        name[1] = 'A';
        name[2] = 'A';
        name[3] = 'A';
        name[4] = '\0';
        id = tracker_add_instrument(&tracker, name, 0);
        assert(id != -1);
        instrument = tracker_get_instrument(&tracker, id);
        assert(instrument != NULL);
        name = tracker_get_instrument_name(&tracker, id);
        assert(name != NULL);
        assert(strcmp(name, "AAAA") == 0);
        name = malloc(INSTRUMENT_NAME_SIZE * sizeof(char));
        name[0] = 'T';
        name[1] = 'E';
        name[2] = 'S';
        name[3] = 'T';
        name[4] = i;
        name[5] = '\0';
        tracker_set_instrument_name(&tracker, id, name);
        assert(strncmp(tracker_get_instrument_name(&tracker, id), "TEST", 4) == 0);
    }
    // TODO: test set/get sample/volume
    // TODO: test trying to add an extra instrument
    puts("Instrument tests concluded");

    // test samples
    puts("Starting sample tests...");
    for (i = 0; i < TRACKER_MAX_SAMPLES; i++) {
        name = malloc(SAMPLE_NAME_SIZE * sizeof(char));
        name[0] = 'A';
        name[1] = 'A';
        name[2] = 'A';
        name[3] = 'A';
        name[4] = '\0';
        data = malloc(100000 * sizeof(unsigned char));
        id = tracker_add_sample(&tracker, name, i, data);
        assert(id != -1);
        printf("%d\n", id);
        sample = tracker_get_sample(&tracker, id);
        assert(sample != NULL);
        name = tracker_get_sample_name(&tracker, id);
        assert(strcmp(name, "AAAA") == 0);
        name = malloc(SAMPLE_NAME_SIZE * sizeof(char));
        name[0] = 'T';
        name[1] = 'E';
        name[2] = 'S';
        name[3] = 'T';
        name[4] = i;
        name[5] = '\0';
        tracker_set_sample_name(&tracker, id, name);
        assert(strncmp(tracker_get_sample_name(&tracker, id), "TEST", 4) == 0);
        assert(tracker_get_sample_data(&tracker, id, &size) != NULL);
        data = malloc(100000 * sizeof(unsigned char));
        for (j = 0; j < SAMPLE_NAME_SIZE - i; j++) {
            data[i] = 'A';
        }
        tracker_set_sample_data(&tracker, id, TRACKER_MAX_SAMPLES - i, data);
        assert(tracker_get_sample_data(&tracker, id, &size) != NULL);
    }
    // TODO: test removing samples and adding more samples
    puts("Sample tests concluded");

    puts("Tracker test has concluded...");
    
    return 0;
}
