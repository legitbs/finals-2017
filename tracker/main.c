#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#ifdef __clemency__
#include "limits.h"
#else
#include <limits.h>
#endif
#include "io.h"
#include "tracker.h"
#include "debug.h"


#define CHUNK_SIZE 8
#define MAX_CHUNKS UCHAR_MAX
#define MAX_PARTS UCHAR_MAX


struct message {
    unsigned char type;
    unsigned int size;  // size of message in chunks
    unsigned char *data;
};


unsigned char *zero_malloc(unsigned int size) {
    unsigned char *data = malloc(size);
    int i;

    for (i = 0; i < size; i++) {
        data[i] = 0;
    }
    return data;
}


int read_chunks(char *buffer, unsigned int n)
{
    return io_readn(buffer, n * CHUNK_SIZE);
}

int write_chunks(char *buffer, unsigned int n)
{
    return io_writen(buffer, n * CHUNK_SIZE);
}


void send_success(int type)
{
    int i;
    char success[CHUNK_SIZE];

    success[0] = type;
    for (i = 1; i < CHUNK_SIZE; i++) {
        success[i] = 0;
    }

    write_chunks(success, 1);
}

void send_failure(int type)
{
    int i;
    char error[CHUNK_SIZE];

    error[0] = type;
    for (i = 1; i < CHUNK_SIZE; i++) {
        error[i] = -1;
    }

    write_chunks(error, 1);
}

void send_response(unsigned int type, unsigned int size, unsigned char *data)
{
    char header[CHUNK_SIZE];
    int offset;
    int i;

    // prepare and send header
    header[0] = type;
    if (size > MAX_CHUNKS) {
        header[1] = size / MAX_CHUNKS;
        header[2] = size % MAX_CHUNKS;
    } else {
        header[1] = 0;
        header[2] = size;
    }
    for (i = 3; i < CHUNK_SIZE; i++) {
        header[i] = 0;
    }
    write_chunks(header, 1);

    // prepare and send data
    if (size > MAX_CHUNKS) {
        for (i = 0; i < size / MAX_CHUNKS; i++) {
            offset = MAX_CHUNKS * CHUNK_SIZE * i;
            write_chunks(data + offset, MAX_CHUNKS);
        }
        offset = MAX_CHUNKS * CHUNK_SIZE * i;
        write_chunks(data + offset, size % MAX_CHUNKS);
    } else {
        write_chunks(data, size);
    }
}


void get_request(struct message *message)
{
    char header[CHUNK_SIZE];
    char *data = NULL;
    int offset;
    int i;

    // get header
    read_chunks(header, 1);

    // get rest of message
    message->type = header[0];
    if (header[1]) {
        // calculate size of and allocate space for multi-part message
        if (header[1] < MAX_PARTS) {
            message->size = (header[1] - 1) * MAX_CHUNKS + header[2];
        } else {
            message->size = MAX_PARTS * MAX_CHUNKS;
        }
        data = malloc(message->size * CHUNK_SIZE * sizeof(unsigned char));

        // receive all parts into data buffer
        for (i = 0; i < header[1]; i++) {
            offset = MAX_CHUNKS * CHUNK_SIZE * i;
            read_chunks(data + offset, MAX_CHUNKS);
        }
        offset = MAX_CHUNKS * CHUNK_SIZE * i;
        read_chunks(data + offset, header[2]);

        // save off the data buffer
        message->data = data;
    } else {
        // allocate space for single message
        if (header[2]) {
            message->size = header[2];
            data = malloc(message->size * CHUNK_SIZE * sizeof(unsigned char));

            // receive data into data buffer
            read_chunks(data, message->size);

            // save off the data buffer
            message->data = data;
        } else {
            message->size = 0;
            message->data = NULL;
        }
    }
}


int process_tracker_message(struct tracker *tracker, struct message *message)
{
    // handle the incoming message
    switch (message->type) {
    // test message please ignore
    case 0:
        send_success(0);
        break;

    // get and set tracker state
    case 1:
    {
        // get the initial volume
        unsigned char *data = zero_malloc(CHUNK_SIZE * sizeof(unsigned char));
        data[0] = tracker_get_initial_volume(tracker);

        // send it to the user and clean up
        send_response(1, 1, data);
        free(data);

        break;
    }
    case 2:
        // fail if there's no data
        if (message->data == NULL || message->size == 0) {
            send_failure(2);
            break;
        }

        // set the initial volume
        tracker_set_initial_volume(tracker, message->data[0]);
        send_success(2);

        break;
    case 3:
    {
        // get the initial tempo
        unsigned char *data = zero_malloc(CHUNK_SIZE * sizeof(unsigned char));
        data[0] = tracker_get_initial_tempo(tracker);

        // send it to the user and clean up
        send_response(3, 1, data);
        free(data);

        break;
    }
    case 4:
        // fail if there's no data
        if (message->data == NULL || message->size == 0) {
            send_failure(4);
            break;
        }

        // set the initial tempo
        tracker_set_initial_tempo(tracker, message->data[0]);
        send_success(4);

        break;
    case 5:
    {
        // get the initial speed
        unsigned char *data = zero_malloc(CHUNK_SIZE * sizeof(unsigned char));
        data[0] = tracker_get_initial_speed(tracker);

        // send it to the user and clean up
        send_response(5, 1, data);
        free(data);

        break;
    }
    case 6:
        // fail if there's no data
        if (message->data == NULL || message->size == 0) {
            send_failure(6);
            break;
        }

        // set the initial speed
        tracker_set_initial_speed(tracker, message->data[0]);
        send_success(6);

        break;
    case 7:
    {
        // get the current pattern length
        unsigned char *data = zero_malloc(CHUNK_SIZE * sizeof(unsigned char));
        data[0] = tracker_get_pattern_length(tracker);

        // send it to the user and clean up
        send_response(7, 1, data);
        free(data);

        break;
    }
    case 8:
        // fail if there's no data
        if (message->data == NULL || message->size == 0) {
            send_failure(8);
            break;
        }

        // set the current pattern length
        tracker_set_pattern_length(tracker, message->data[0]);
        send_success(8);

        break;

    // frame manipulation
    case 16:
    {
        unsigned char *frame;

        // fail if there's no data
        if (message->data == NULL || message->size == 0) {
            send_failure(16);
            break;
        }

        // add the frame to the tracker
        frame = zero_malloc(TRACKER_MAX_CHANNELS * sizeof(unsigned char));
        memcpy(frame, message->data, TRACKER_MAX_CHANNELS * sizeof(unsigned char));
        if (tracker_add_frame(tracker, frame) == TRACKER_SUCCESS) {
            send_success(16);
        } else {
            send_failure(16);
        }

        break;
    }
    case 17:
        // fail if there's no data
        if (message->data == NULL || message->size == 0) {
            send_failure(17);
            break;
        }

        // remove the frame from the tracker
        if (tracker_remove_frame(tracker, message->data[0]) == 0) {
            send_success(17);
        } else {
            send_failure(17);
        }

        break;
    case 18:
    {
        unsigned char *frame;

        // fail if there's no data
        if (message->data == NULL || message->size == 0) {
            send_failure(18);
            break;
        }

        // return the current frame to the user
        frame = tracker_get_frame(tracker, message->data[0]);
        if (frame == NULL) {
            send_failure(18);
        } else {
            send_response(18, 1, frame);
        }

        break;
    }
    case 19:
    {
        unsigned char *frame;

        // fail if there's no data or not enough data
        if (message->data == NULL || message->size == 0 || message->size < 2) {
            send_failure(19);
            break;
        }

        // set the current frame in the tracker
        frame = malloc(TRACKER_MAX_CHANNELS * sizeof(unsigned char));
        memcpy(frame, message->data, TRACKER_MAX_CHANNELS * sizeof(unsigned char));
        if (tracker_set_frame(tracker, message->data[0],frame) == TRACKER_SUCCESS) { 
            send_success(19);
        } else {
            send_failure(19);
        }

        break;
    }
    case 20:
    {
        unsigned char *data;

        // fail if there's not enough data
        if (message->data == NULL || message->size == 0) {
            send_failure(20);
            break;
        }

        // get the current number of frames
        data = zero_malloc(CHUNK_SIZE * sizeof(unsigned char));
        data[0] = tracker_get_total_frames(tracker);

        // send it to the user and clean up
        send_response(20, 1, data);
        free(data);

        break;
    }

    // channel manipulation
    case 32:
    {
        char *name;

        // fail if there's not enough data
        if (message->data == NULL || message->size == 0) {
            send_failure(32);
            break;
        }

        // get the current channel name
        name = tracker_get_channel_name(tracker, message->data[0]);
        if (name == NULL) {
            send_failure(32);
            break;
        }

        // return it to the user and clean up
        send_response(32, CHANNEL_NAME_SIZE/CHUNK_SIZE, name);
        if (name) {
            free(name);
        }

        break;
    }
    case 33:
    {
        char *name;
        unsigned int size;

        // fail if there's not enough data
        if (message->data == NULL || message->size == 0 || message->size < 2) {
            send_failure(33);
            break;
        }

        // get the channel name from the incoming message
        size = (message->size - 1 <= CHANNEL_NAME_SIZE/CHUNK_SIZE) ? message->size - 1 : CHANNEL_NAME_SIZE/CHUNK_SIZE;
        name = zero_malloc(size * CHUNK_SIZE * sizeof(char));
        memcpy(name, message->data[CHUNK_SIZE], size * CHUNK_SIZE * sizeof(char));

        // set the channel name in the tracker
        if (tracker_set_channel_name(tracker, message->data[0], name) == TRACKER_SUCCESS) {
            send_success(33);
        } else {
            send_failure(33);
        }

        break;
    }
    case 34:
    {
        unsigned char *data;
        int tmp;

        // fail if there's not enough data
        if (message->data == NULL || message->size == 0) {
            send_failure(34);
            break;
        }

        // get the current volume for the selected channel
        data = zero_malloc(CHUNK_SIZE * sizeof(unsigned char));
        tmp = tracker_get_channel_volume(tracker, message->data[0]);

        // if there was a problem (like the channel ID not being valid), report an error
        if (tmp < 0) {
            send_failure(34);
            break;
        }

        // if there wasn't a problem, send it to the user and clean up
        data[0] = tmp;
        send_response(34, 1, data);
        if (data) {
            free(data);
        }

        break;
    }
    case 35:
        // set the channel volume if we've been sent an ID and value
        if (message->data != NULL && message->size != 0 &&
            tracker_set_channel_volume(tracker, message->data[0], message->data[1]) == TRACKER_SUCCESS)
        {
            send_success(35);
        } else {
            send_failure(35);
        }

        break;
    case 36:
        // mute the channel if we've been sent an ID
        if (message->data != NULL && message->size != 0 && tracker_mute_channel(tracker, message->data[0]) == 0) {
            send_success(36);
        } else {
            send_failure(36);
        }

        break;
    case 37:
        // unmute the channel if we've been sent an ID
        if (message->data != NULL && message->size != 0 && tracker_unmute_channel(tracker, message->data[0]) == 0) {
            send_success(37);
        } else {
            send_failure(37);
        }

        break;

    // pattern and event manipulation
    case 48:
    {
        unsigned char *output;
        int tmp;

        // create the pattern and return the ID to the user if we were successful
        tmp = tracker_create_pattern(tracker);
        if (tmp < 0) {
            send_failure(48);
        } else {
            output = zero_malloc(CHUNK_SIZE * sizeof(unsigned char));
            output[0] = tmp;
            send_response(48, 1, output);
        }

        break;
    }
    case 49:
    {
        struct event *event;
        char *output;

        // fail if there's not enough data
        if (message->data == NULL || message->size == 0) {
            send_failure(49);
            break;
        }

        // get the selected pattern
        event = tracker_get_event_from_pattern(tracker, message->data[0], message->data[1]);
        if (event == NULL) {
            send_failure(49);
            break;
        }

        // serialize it to the user
        output = zero_malloc(CHUNK_SIZE * sizeof(unsigned char));
        output[0] = event->note;
        output[1] = event->octave;
        output[2] = event->volume;
        output[3] = event->instrument;
        output[4] = event->effect;
        output[5] = event->parameter;
        send_response(49, 1, output);

        // clean up
        if (output) {
            free(output);
        }

        break;
    }
    case 50:
        // fail if there's not enough data
        if (message->data == NULL || message->size == 0) {
            send_failure(50);
            break;
        }

        // destroy the selected pattern if it exists
        tracker_destroy_pattern(tracker, message->data[0]);
        send_success(50);

        break;
    case 51:
        // fail if there's not enough data
        if (message->data == NULL || message->size == 0) {
            send_failure(51);
            break;
        }

        // add the event to the pattern and report success/failure to the user
        if (tracker_add_event_to_pattern(tracker, message->data[0], message->data[1], message->data[2], message->data[3],
                                         message->data[4], message->data[5], message->data[6], message->data[7]) == 0)
        {
            send_success(51);
        } else {
            send_failure(51);
        }

        break;
    case 52:
        // fail if there's not enough data
        if (message->data == NULL || message->size == 0) {
            send_failure(52);
            break;
        }

        // remove the selected event from the selected pattern if it exists
        tracker_remove_event_from_pattern(tracker, message->data[0], message->data[1]);
        send_success(52);

        break;

    // instrument manipulation
    case 64:
    {
        char *name;
        int size;

        // fail if there's not enough data
        if (message->data == NULL || message->size == 0 || message->size < 2) {
            send_failure(64);
            break;
        }

        // get the channel name from the incoming message
        size = (message->size - 1 <= INSTRUMENT_NAME_SIZE/CHUNK_SIZE) ? message->size - 1 : INSTRUMENT_NAME_SIZE/CHUNK_SIZE;
        name = zero_malloc(size * CHUNK_SIZE * sizeof(char));
        memcpy(name, message->data[CHUNK_SIZE], size * CHUNK_SIZE * sizeof(char));

        // add instrument to the tracker
        tracker_add_instrument(tracker, name, message->data[0]);
        send_success(64);

        break;
    }
    case 65:
    {
        char *name;

        // fail if there's not enough data
        if (message->data == NULL || message->size == 0) {
            send_failure(65);
            break;
        }

        // get the selected instrument name
        name = tracker_get_instrument_name(tracker, message->data[0]);
        if (name == NULL) {
            send_failure(65);
            break;
        }

        // return it to the user and clean up
        send_response(32, INSTRUMENT_NAME_SIZE/CHUNK_SIZE, name);
        free(name);

        break;
    }
    case 66:
    {
        char *name;
        unsigned int size;

        // fail if there's not enough data
        if (message->data == NULL || message->size == 0 || message->data[0] == NULL || message->size < 2) {
            send_failure(66);
            break;
        }

        // get the channel name from the incoming message
        size = (message->size - 1 <= INSTRUMENT_NAME_SIZE/CHUNK_SIZE) ? message->size - 1 : INSTRUMENT_NAME_SIZE/CHUNK_SIZE;
        name = zero_malloc(size * CHUNK_SIZE * sizeof(char));
        memcpy(name, message->data[CHUNK_SIZE], size * CHUNK_SIZE * sizeof(char));

        // set the channel name in the tracker
        tracker_set_instrument_name(tracker, message->data[0], name);
        send_success(66);

        break;
    }
    case 67:
    {
        struct sample *sample;
        char *output;

        // fail if there's not enough data
        if (message->data == NULL || message->size == 0) {
            send_failure(67);
            break;
        }

        // get the selected instrument
        sample = tracker_get_instrument_sample(tracker);
        if (sample == NULL) {
            send_failure(67);
            break;
        }

        // return the sample's id
        output = zero_malloc(CHUNK_SIZE * sizeof(unsigned char));
        output[0] = sample->id;
        send_response(67, 1, output);

        // clean up
        if (output) {
            free(output);
        }

        break;
    }
    case 68:
        // fail if there's not enough data
        if (message->data == NULL || message->size == 0) {
            send_failure(68);
            break;
        }

        // set the sample for the selected instrument
        tracker_set_instrument_sample(tracker, message->data[0], message->data[1]);
        send_success(68);

        break;
    case 69:
    {
        char *output;

        // fail if there's not enough data
        if (message->data == NULL || message->size == 0) {
            send_failure(69);
            break;
        }

        // get the current volume for the selected instrument and return it to the user
        output = zero_malloc(CHUNK_SIZE * sizeof(unsigned char));
        output[0] = tracker_get_instrument_volume(tracker, message->data[0]);
        send_response(69, 1, output);

        // clean up
        free(output);

        break;
    }
    case 70:
        // fail if there's not enough data
        if (message->data == NULL || message->size == 0) {
            send_failure(70);
            break;
        }

        // set the volume for the selected instrument
        tracker_set_instrument_volume(tracker, message->data[0], message->data[1]);
        send_success(70);

        break;

    // sample manipulation
    case 80:
    {
        unsigned char *output = NULL;
        char *name = NULL;
        unsigned char *data = NULL;
        int data_size;

        // fail if there's no data or the name/data for the sample are empty or too large
        if (message->data == NULL || message->data[0] == 0 || message->data[0] > 4 || message->size < 3) {
            send_failure(80);
            break;
        }
        data_size = *(int *)&message->data[1];
        if (data_size == 0 || data_size > (message->size - 1 - message->data[0]) * CHUNK_SIZE) {
            send_failure(80);
            break;
        }

        // copy the name out of the message
        name = malloc(message->data[0] * CHUNK_SIZE * sizeof(char));
        memcpy(name, &message->data[CHUNK_SIZE], message->data[0] * CHUNK_SIZE * sizeof(char));

        // copy the sample data out of the message
        data = malloc(data_size * sizeof(unsigned char));
        memcpy(data, &message->data[CHUNK_SIZE + message->data[0] * CHUNK_SIZE], data_size * sizeof(unsigned char));
        
        // add the sample
        output = zero_malloc(CHUNK_SIZE * sizeof(unsigned char));
        output[0] = tracker_add_sample(tracker, name, data_size, data);
        send_response(80, 1, output);

        // clean up
        free(output);
        free(name); // tracker_add_sample will memcpy the data out of here for us
        break;
    }
    case 81:
    {
        char *name = NULL;

        // fail if there's no data or if the data is too small
        if (!message->data || message->size < 1) {
            send_failure(81);
            break;
        }

        // get the name associated with the sample
        name = tracker_get_sample_name(tracker, message->data[0]);
        if (name) {
            send_response(81, SAMPLE_NAME_SIZE/CHUNK_SIZE, name);
        } else {
            send_failure(81);
        }

        break;
    }
    case 82:
        // fail if there's no data or if the data is too small
        if (!message->data || message->size < 2) {
            send_failure(82);
            break;
        }

        // set the sample name
        tracker_set_sample_name(tracker, message->data[0], &message->data[CHUNK_SIZE]);
        send_success(82);

        break;
    case 83:
    {
        unsigned char *data;
        unsigned char *output;
        int size;

        // fail if there's no data
        if (message->data == NULL || message->size == 0) {
            send_failure(83);
            break;
        }

        // get sample data
        data = tracker_get_sample_data(tracker, message->data[0], &size);
        if (size == 0 || data == NULL) {
            send_failure(83);
        }

        // send it to the user
        if (size % CHUNK_SIZE > 0) {
            size += CHUNK_SIZE - (size % CHUNK_SIZE);
            output = zero_malloc(size * sizeof(unsigned char));
            memcpy(output, data, size);
            send_response(83, size/CHUNK_SIZE, output);
        } else {
            send_response(83, size/CHUNK_SIZE, data);
        }

        // clean up
        if (output) {
            free(output);
        }

        break;
    }
    case 84:
    {
        unsigned char empty[CHUNK_SIZE] = { 0 };
        unsigned int size;
        unsigned char *data;

        // fail if there's no data or if the data is too small
        if (message->data == NULL || message->size < 1) {
            send_failure(84);
            break;
        }

        // prepare the sample data
        size = *(int *)&message->data[1];
        data = &message->data[CHUNK_SIZE];
        if (size > (message->size - 1) * CHUNK_SIZE) {
            size = (message->size - 1) * CHUNK_SIZE;  // handle overflow
        } else if (size == 0) {
            data = empty;
        }

        // set the sample data
        tracker_set_sample_data(tracker, message->data[0], size, data);
        send_success(84);
        break;
    }
    case 85:
        // fail if there's no data or if the data is too small
        if (message->data == NULL || message->size < 1) {
            send_failure(85);
            break;
        }

        // remove the selected sample
        tracker_remove_sample(tracker, message->data[0]);
        send_success(85);

        break;

    // signal processing
    case 96:
        // TODO: actually implement this
        //send_success(96);
        break;

    // miscellaneous
    case 144:
        send_success(144);
        return 1;  // exit the program

    // default failure case
    default:
        break;
    }

    return 0;
}


int perform_handshake()
{
    char input[CHUNK_SIZE];
    char rdata[CHUNK_SIZE] = { 'F', 'U', 'Z', 'Y', 'L', 'L', '1', '7' };
    char output[CHUNK_SIZE];
    int i;

    // start handshake
    read_chunks(input, 1);
    memcpy(output, input, CHUNK_SIZE);
    for (i = 0; i < CHUNK_SIZE; i++) {
        //rdata[i] = rand();
        output[i] ^= rdata[i];
    }
    send_response(145, 1, output);

    // finish handshake
    read_chunks(input, 1);
    for (i = 0; i < CHUNK_SIZE; i++) {
        if (input[i] != rdata[i]) {
            send_failure(-1);
            return -1;
        }
    }
    send_success(-1);

    return 0;
}


int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    struct tracker tracker;
    int exit;

#ifndef __clemency__
    setvbuf(stdout, NULL, _IONBF, 0);
#endif

    // perform initial connection handshake
    //srand(time(0));
    if (perform_handshake() != 0) {
        return -1;
    }

    // receive initial state
    tracker_initialize(&tracker);

    while (1) {
        // check for user requests
        struct message *message = malloc(sizeof(struct message));
        get_request(message);

        // if we got a request, process it
        if (message) {
            exit = process_tracker_message(&tracker, message);
            if (message->data) {
                free(message->data);
            }
            free(message);
        }

        // handle exit case
        if (exit) { break; }
    }

    return 0;
}

