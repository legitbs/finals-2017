#include "sample.h"

int sample_get_id(struct sample *sample)
{
    if (sample) {
        return sample->id;
    } else {
        return -1;
    }
}

char *sample_get_name(struct sample* sample)
{
    if (sample) {
        return sample->name;
    } else {
        return NULL;
    }
}

void sample_set_name(struct sample *sample, char *name)
{
    int i;

    if (sample) {
        int size = strlen(name);
        if (size > SAMPLE_NAME_SIZE) {
            size = SAMPLE_NAME_SIZE;
        }
        for (i = 0; i <= size; i++) {
            sample->name[i] = name[i];
        }
    }
}

unsigned char *sample_get_data(struct sample *sample, unsigned int *size)
{
    if (sample) {
        *size = sample->size;
        return sample->data;
    } else {
        *size = 0;
        return NULL;
    }
}

void sample_set_data(struct sample *sample, unsigned int size, unsigned char *data)
{
    if (size > SAMPLE_MAX_DATA_SIZE) {
        size = SAMPLE_MAX_DATA_SIZE;
    }

    if (sample) {
        if (size != 0 && size > sample->size) {
            sample->size = size;
            if (sample->data) {
                free(sample->data);
            }
            sample->data = malloc(size * sizeof(unsigned char));
            memcpy(sample->data, data, size);
        } else if (size != 0 && size < sample->size) {
            sample->size = size;
            memcpy(sample->data, data, size);
        } else {
            sample->size = size;
            sample->data = data;
        }
    }
}

void sample_add_reference(struct sample *sample)
{
	if (sample) {
		sample->references++;
	}
}

void sample_remove_reference(struct sample *sample)
{
	if (sample) {
		sample->references--;
	}
}
