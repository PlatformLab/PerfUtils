/* Copyright (c) 2017 Stanford University
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
#ifndef PERFUTILS_STATS_MINIMAL_H
#define PERFUTILS_STATS_MINIMAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This file exists to make the computeStatistics function available from C
 * without an extra wrapper around the stats component of PerfUtils.
 */
struct Statistics {
    size_t count;
    uint64_t average;
    uint64_t stddev;
    uint64_t min;
    uint64_t median;
    uint64_t P10;
    uint64_t P20;
    uint64_t P30;
    uint64_t P40;
    uint64_t P50;
    uint64_t P60;
    uint64_t P70;
    uint64_t P80;
    uint64_t P90;
    uint64_t P99;
    uint64_t P999;
    uint64_t P9999;
    uint64_t max;
};

struct Statistics computeStatistics(uint64_t* rawdata, size_t count);


/**
 * Apply a transformation function on all statistics, usually to change the
 * units. It is assumed that the units on all statistics are initially
 * identical.
 */
struct Statistics transformStatistics(struct Statistics stats, uint64_t (*function)(uint64_t));

/**
 * Print out all the statistics in CSV format.
 */
void printStatistics(struct Statistics stats, const char* label);

#ifdef __cplusplus
}
#endif

#endif // PERFUTILS_STATS_MINIMAL_H
