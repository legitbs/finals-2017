/*
 * io.c | Input/Output Wrappers Adapted from libctf
 *
 * Copyright (c) 2012-2017 Alexander Taylor <ajtaylor@fuzyll.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "io.h"

/*
 * Reads from a file descriptor until given length is reached.
 * Returns number of bytes received.
 */
int io_readn(void *_msg, const unsigned int len)
{
    int prev = 0;  // previous amount of bytes we read
    unsigned int count = 0;
    char *msg = (char *)_msg;

    if (msg && len) {
        // keep reading bytes until we've got the whole message
        for (count = 0; count < len; ) {
#ifdef __clemency__
            prev = read(msg + count, len - count);
            if (prev == 0) {
                wait();
            }
#else
            prev = read(STDIN_FILENO, msg + count, len - count);
            if (prev <= 0) {
                break;
            }
#endif
            count += prev;  // moved the loop counter down here to get around a bug in ncc's code generation
        }
    }

    return count;
}


/*
 * Reads from a file descriptor until a newline or maximum length is reached.
 * Returns number of bytes read, including the newline (which is now NULL).
 */
int io_readsn(char *msg, const unsigned int len)
{
    unsigned int count = 0;
    char tmp;  // temporary storage for each character read

    if (msg && len) {
        for (count = 0; count < len; count++) {
            // read character
#ifdef __clemency__
            if (read(&tmp, 1) <= 0) {
                wait();
            }
#else
            if (read(STDIN_FILENO, &tmp, 1) <= 0) {
                break;
            }
#endif

            // break loop if we received a newline
            if (tmp == '\n') {
                msg[count] = '\0';
                break;
            }

            // add character to our message
            msg[count] = tmp;
        }
    }

    return count;
}


/*
 * Wrapper for io_writen() that does strlen() for you.
 * Returns number of bytes written (or <= 0 for failure).
 */
int io_writes(const char *msg)
{
    return io_writen(msg, strlen(msg));
}


/*
 * Writes a given message of a given length to a given file descriptor.
 * Returns number of bytes written (or <= 0 for failure).
 */
int io_writen(const void *_msg, const unsigned int len)
{
    int prev = 0;  // previous amount of bytes we wrote
    unsigned int count = 0;
    const char *msg = (const char*) _msg;

    // write entire message (in chunks if we have to)
    if (msg && len) {
        for (count = 0; count < len; ) {
#ifdef __clemency__
            prev = write(msg + count, len - count);
#else
            prev = write(STDOUT_FILENO, msg + count, len - count);
            if (prev <= 0) {
                return prev;
            }
#endif
            count += prev;  // moved the loop counter down here to get around a bug in ncc's code generation
        }
    }

    return count;
}
