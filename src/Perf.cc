/* Copyright (c) 2012 Stanford University
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
#include "Perf.h"

#include <string.h>
#include <stdint.h>

#include "Cycles.h"

namespace PerfUtils {

    /**
     * Run the given function for numIterations, and compute statistics on the run times.
     */
    Statistics bench(void (*function)(void), int numIterations) {
        uint64_t* latencies = new uint64_t[numIterations];
        uint64_t startTime;

        // Page in the memory
        memset(latencies, 0, numIterations * sizeof(uint64_t));

        for (int i = 0; i < numIterations; i++) {
            startTime = Cycles::rdtsc();
            function();
            latencies[i] = Cycles::rdtsc() - startTime;
        }

        Statistics stats = computeStatistics(latencies, numIterations);
        delete[] latencies;
        return stats;
    }

    /**
     * Run the given function for numIterations, and compute statistics on the
     * times reported by the function itself. The only argument to the function
     * is an output parameter where the measured time should be placed.
     */
    Statistics manualBench(void (*function)(uint64_t*), int numIterations) {
        uint64_t* latencies = new uint64_t[numIterations];

        // Page in the memory
        memset(latencies, 0, numIterations * sizeof(uint64_t));
        for (int i = 0; i < numIterations; i++) {
            function(&latencies[i]);
        }

        Statistics stats = computeStatistics(latencies, numIterations);
        delete[] latencies;
        return stats;
    }
}
