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

#ifndef PERFUTILS_STATS_H
#define PERFUTILS_STATS_H

#include <stddef.h>
#include <stdint.h>

struct Statistics {
    size_t count;
    uint64_t average;
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

Statistics computeStatistics(uint64_t* rawdata, size_t count);

void printStatistics(const char* label, uint64_t* rawdata, size_t count,
                     const char* datadir = NULL);

void printHistogram(uint64_t* rawdata, size_t count, uint64_t lowerbound,
                    uint64_t upperbound, uint64_t step);

#endif
