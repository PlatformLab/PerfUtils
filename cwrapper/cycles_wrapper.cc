/* Copyright (c) 2018 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <errno.h>
#include <stdio.h>

#include "Cycles.h"

using PerfUtils::Cycles;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function is the wrapper for Cycles::init.
 */
void
cycles_init() {
    Cycles::init();
}

uint64_t
cycles_rdtsc() {
    return Cycles::rdtsc();
}

/**
 * This function wraps Cycles::toSeconds().
 */
double
cycles_to_seconds(uint64_t cycles) {
    return Cycles::toSeconds(cycles);
}
uint64_t
cycles_from_seconds(double seconds) {
    return Cycles::fromSeconds(seconds);
}
uint64_t
cycles_to_milliseconds(uint64_t cycles) {
    return Cycles::toMilliseconds(cycles);
}
uint64_t
cycles_from_milliseconds(uint64_t ms) {
    return Cycles::fromMilliseconds(ms);
}
uint64_t
cycles_to_microseconds(uint64_t cycles) {
    return Cycles::toMicroseconds(cycles);
}
uint64_t
cycles_from_microseconds(uint64_t us) {
    return Cycles::fromMicroseconds(us);
}
uint64_t
cycles_to_nanoseconds(uint64_t cycles) {
    return Cycles::toNanoseconds(cycles);
}
uint64_t
cycles_from_nanoseconds(uint64_t ns) {
    return Cycles::fromNanoseconds(ns);
}
void
cycles_sleep(uint64_t us) {
    Cycles::sleep(us);
}

#ifdef __cplusplus
}
#endif
