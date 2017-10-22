#include "channel.h"

void channel_initialize(struct channel *channel, unsigned int volume)
{
    channel->name = malloc(CHANNEL_NAME_SIZE * sizeof(char));
    channel->volume = volume;
    channel->muted = 0;
}

void channel_set_name(struct channel *channel, char *name)
{
    strncpy(channel->name, name, CHANNEL_NAME_SIZE);
}

char *channel_get_name(struct channel *channel)
{
    return channel->name;
}

void channel_set_volume(struct channel *channel, unsigned int volume)
{
    channel->volume = volume;
}

unsigned int channel_get_volume(struct channel *channel)
{
    return channel->volume;
}

void channel_mute(struct channel *channel)
{
    channel->muted = 1;
}

void channel_unmute(struct channel *channel)
{
    channel->muted = 0;
}

unsigned int channel_is_muted(struct channel *channel)
{
    return channel->muted == 1;
}
