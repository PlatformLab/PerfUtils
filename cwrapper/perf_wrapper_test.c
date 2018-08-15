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

#include "perf_wrapper.h"
#include <stdio.h>

#define RED(X)   "\033[31m"  X "\033[0m"
#define GREEN(X) "\033[32m"  X "\033[0m"

void fixedCycles(int N) {
    for (int i = 0; i < N; i++) {
        asm volatile("nop");
    }
}

void fiveHundredCycles() {
    fixedCycles(500);
}

uint64_t half(uint64_t n) {
    return n / 2;
}

void fixedPerformance(uint64_t* N) {
    *N = 7;
}

int
main() {
    Statistics stats = bench(fiveHundredCycles, 100000);
    stats = transformStatistics(stats, half);
    if (stats.count == 100000 && stats.count > stats.min) {
        puts(GREEN("perf_wrapper_test::bench PASSED"));
    } else {
        puts(RED("perf_wrapper_test::bench FAILED"));
    }

    stats = manualBench(fixedPerformance, 100000);
    if (stats.count == 100000 && stats.average == 7 && stats.median == 7 && stats.min == 7 && stats.max == 7 && stats.stddev == 0) {
        puts(GREEN("perf_wrapper_test::manualBench PASSED"));
    } else {
        puts(RED("perf_wrapper_test::manualBench FAILED"));
    }
}
