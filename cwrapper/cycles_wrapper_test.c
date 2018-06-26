/* Copyright (c) 2018 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright
 * notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "cycles_wrapper.h"
#include <unistd.h>

int
main() {
    cycles_init();
    uint64_t startTime = cycles_rdtsc();
    uint64_t diff = cycles_rdtsc() - startTime;
    printf("%lu cycles = %lu ns\n", diff, cycles_to_nanoseconds(diff));
    usleep(1000);
    diff = cycles_rdtsc() - startTime;
    printf("%lu cycles = %lu us\n", diff, cycles_to_microseconds(diff));
    printf("%lu cycles = %lu ms\n", diff, cycles_to_milliseconds(diff));
    printf("%lu cycles = %f s\n", diff, cycles_to_seconds(diff));

    printf("1 s = %lu cycles\n", cycles_from_seconds(1));
    printf("1 ms = %lu cycles\n", cycles_from_milliseconds(1));
    printf("1 us = %lu cycles\n", cycles_from_microseconds(1));
    printf("1 ns = %lu cycles\n", cycles_from_nanoseconds(1));
    return 0;
}
