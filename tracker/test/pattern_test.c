#include <stdio.h>
#include "pattern.h"
#include "event.h"
#ifdef __clemency__
#include "assert.h"
#else
#include <assert.h>
#endif

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    puts("Starting pattern test...");
    fflush(stdout);
    
    // create test values
    struct event *event1 = event_create(1, 1, 0, 0, 0, 0);
    struct event *event2 = event_create(2, 2, 0, 0, 0, 0);
    struct event *event3 = event_create(3, 3, 0, 0, 0, 0);
    struct event *event4 = event_create(4, 4, 0, 0, 0, 0);
    struct event *event5 = event_create(5, 5, 0, 0, 0, 0);

    // test creating and initializing a pattern
    struct pattern *pattern = pattern_create();
    pattern_resize(pattern, 64);
    pattern_set_id(pattern, 1);

    // test pattern_set_id and pattern_get_id
    assert(pattern_get_id(pattern) == 1);

    // test pattern_add_event, pattern_get_event, and pattern_remove_event
    pattern_add_event(pattern, 0, event1);
    pattern_add_event(pattern, 100, event2);
    pattern_add_event(pattern, 63, event3);
    pattern_add_event(pattern, 10, event4);
    struct event *test = pattern_get_event(pattern, 0);
    assert(test == event1);
    assert(test->note == 1);
    assert(test->octave == 1);
    assert(pattern_get_event(pattern, 10) == event4);
    assert(pattern_get_event(pattern, 25) == NULL);
    assert(pattern_get_event(pattern, 63) == event3);
    assert(pattern_get_event(pattern, 100) == NULL);
    pattern_add_event(pattern, 10, event5);
    assert(pattern_get_event(pattern, 10) == event5);
    pattern_remove_event(pattern, 10);
    assert(pattern_get_event(pattern, 10) == NULL);

    // test pattern_resize with data
    pattern_resize(pattern, 32);
    //assert(pattern_get_event(pattern, 63) == NULL);

    pattern_destroy(pattern);
    puts("Pattern test has concluded");
    fflush(stdout);

    return 0;
}
