#include <stdio.h>
#include "instrument.h"
#include "sample.h"
#ifdef __clemency__
#include "assert.h"
#else
#include <assert.h>
#endif

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    puts("Starting instrument test...");
    fflush(stdout);

    // create test values
    struct sample *sample1 = malloc(sizeof(struct sample));
    struct sample *sample2 = malloc(sizeof(struct sample));
    struct sample *sample3 = malloc(sizeof(struct sample));
    char *sname1 = "sample1";
    char *sname2 = "sample2";
    char *sname3 = "sample3";
    struct instrument *instrument1 = malloc(sizeof(struct instrument));
    struct instrument *instrument2 = malloc(sizeof(struct instrument));
    struct instrument *instrument3 = malloc(sizeof(struct instrument));
    struct instrument *instrument4 = malloc(sizeof(struct instrument));
    struct instrument *instrument5 = malloc(sizeof(struct instrument));
    char *iname1 = "instrument1";
    char *iname2 = "instrument2";
    char *iname3 = "instrument3";
    char *iname4 = "instrument4";
    char *iname5 = "instrument5";

    // test sample_set_name, sample_get_name, sample_set_data, sample_get_data
    sample_set_name(sample1, sname1);
    sample_set_name(sample2, sname2);
    sample_set_name(sample3, sname3);
    //sample_set_data
    assert(!strcmp(sample_get_name(sample1), sname1));
    assert(!strcmp(sample_get_name(sample2), sname2));
    assert(!strcmp(sample_get_name(sample3), sname3));
    //sample_get_data

    // test instrument_set_name, instrument_get_name
    instrument_set_name(instrument1, iname1);
    instrument_set_name(instrument2, iname2);
    instrument_set_name(instrument3, iname3);
    instrument_set_name(instrument4, iname4);
    instrument_set_name(instrument5, iname5);
    assert(!strcmp(instrument_get_name(instrument1), iname1));
    assert(!strcmp(instrument_get_name(instrument2), iname2));
    assert(!strcmp(instrument_get_name(instrument3), iname3));
    assert(!strcmp(instrument_get_name(instrument4), iname4));
    assert(!strcmp(instrument_get_name(instrument5), iname5));

    // test instrument_set_sample, instrument_get_sample
    instrument_set_sample(instrument1, sample1);
    instrument_set_sample(instrument2, sample1);
    instrument_set_sample(instrument3, sample1);
    instrument_set_sample(instrument4, sample1);
    instrument_set_sample(instrument5, sample1);
    assert(instrument_get_sample(instrument1) == sample1);
    assert(instrument_get_sample(instrument2) == sample1);
    assert(instrument_get_sample(instrument3) == sample1);
    assert(instrument_get_sample(instrument4) == sample1);
    assert(instrument_get_sample(instrument5) == sample1);
    assert(sample1->references == 5);
    instrument_set_sample(instrument2, sample2);
    assert(instrument_get_sample(instrument2) == sample2);
    assert(sample1->references == 4);
    assert(sample2->references == 1);
    assert(sample3->references == 0);
    instrument_set_sample(instrument3, sample3);
    assert(instrument_get_sample(instrument3) == sample3);
    assert(sample1->references == 3);
    assert(sample2->references == 1);
    assert(sample3->references == 1);

    // test instrument_set_volume, instrument get_volume
    instrument_set_volume(instrument1, INSTRUMENT_MAX_VOLUME);
    instrument_set_volume(instrument2, 0);
    instrument_set_volume(instrument3, 10);
    instrument_set_volume(instrument4, -1);
    instrument_set_volume(instrument5, 100);
    assert(instrument_get_volume(instrument1) == INSTRUMENT_MAX_VOLUME);
    assert(instrument_get_volume(instrument2) == 0);
    assert(instrument_get_volume(instrument3) == 10);
    assert(instrument_get_volume(instrument4) == INSTRUMENT_MAX_VOLUME);
    assert(instrument_get_volume(instrument5) == INSTRUMENT_MAX_VOLUME);

    // test instrument_set_loop, instrument_get_loop
    // TODO

    free(instrument1);
    free(instrument2);
    free(instrument3);
    free(instrument4);
    free(instrument5);
    free(sample1);
    free(sample2);
    free(sample3);
    puts("Instrument test has concluded");
    fflush(stdout);

    return 0;
}
