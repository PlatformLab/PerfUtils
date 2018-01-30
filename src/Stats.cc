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

#include "Stats.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mkdir.h"

int
compare(const void* a, const void* b) {
    if (*(const uint64_t*)a == *(const uint64_t*)b) return 0;
    return *(const uint64_t*)a < *(const uint64_t*)b ? -1 : 1;
}

Statistics
computeStatistics(uint64_t* rawdata, size_t count) {
    Statistics stats;
    if (count == 0) return stats;
    qsort(rawdata, count, sizeof(uint64_t), compare);
    uint64_t sum = 0;
    for (size_t i = 0; i < count; i++) sum += rawdata[i];
    stats.average = sum / count;
    stats.min = rawdata[0];
    stats.median = rawdata[count / 2];
    stats.P10 = rawdata[static_cast<int>(static_cast<double>(count) * 0.1)];
    stats.P20 = rawdata[static_cast<int>(static_cast<double>(count) * 0.2)];
    stats.P30 = rawdata[static_cast<int>(static_cast<double>(count) * 0.3)];
    stats.P40 = rawdata[static_cast<int>(static_cast<double>(count) * 0.4)];
    stats.P50 = rawdata[static_cast<int>(static_cast<double>(count) * 0.5)];
    stats.P60 = rawdata[static_cast<int>(static_cast<double>(count) * 0.6)];
    stats.P70 = rawdata[static_cast<int>(static_cast<double>(count) * 0.7)];
    stats.P80 = rawdata[static_cast<int>(static_cast<double>(count) * 0.8)];
    stats.P90 = rawdata[static_cast<int>(static_cast<double>(count) * 0.9)];
    stats.P99 = rawdata[static_cast<int>(static_cast<double>(count) * 0.99)];
    stats.P999 = rawdata[static_cast<int>(static_cast<double>(count) * 0.999)];
    stats.P9999 =
        rawdata[static_cast<int>(static_cast<double>(count) * 0.9999)];
    stats.max = rawdata[count - 1];
    return stats;
}

void
printStatistics(const char* label, uint64_t* rawdata, size_t count,
                const char* datadir) {
    static bool headerPrinted = false;
    if (!headerPrinted) {
        puts(
            "Benchmark,Count,Avg,Median,Min,99%,"
            "99.9%,99.99%,Max");
        headerPrinted = true;
    }
    Statistics stats = computeStatistics(rawdata, count);
    printf("%s,%zu,%lu,%lu,%lu,%lu,%lu,%lu,%lu\n", label, count, stats.average,
           stats.median, stats.min, stats.P99, stats.P999, stats.P9999,
           stats.max);

    // Dump the data out
    if (datadir != NULL) {
        char buf[1024];
        snprintf(buf, sizeof(buf), "%s/%s", datadir, label);
        ensureParents(buf);
        FILE* fp = fopen(buf, "w");
        for (size_t i = 0; i < count; i++) fprintf(fp, "%lu\n", rawdata[i]);
        fclose(fp);
    }
}

void
printHistogram(uint64_t* rawdata, size_t count, uint64_t lowerbound,
               uint64_t upperbound, uint64_t step) {
    size_t numBuckets = (upperbound - lowerbound) / step + 1;
    uint64_t* buckets =
        reinterpret_cast<uint64_t*>(calloc(numBuckets, sizeof(uint64_t)));
    for (size_t i = 0; i < count; i++) {
        bool foundBucket = false;
        for (uint64_t k = lowerbound; k < upperbound; k += step) {
            if (rawdata[i] < k + step) {
                buckets[(k - lowerbound) / step]++;
                foundBucket = true;
                break;
            }
        }
        if (!foundBucket) {
            buckets[numBuckets - 1]++;
        }
    }

    for (uint64_t k = lowerbound; k < upperbound; k += step) {
        printf("%lu-%lu: %lu\n", k, k + step, buckets[(k - lowerbound) / step]);
    }
    printf("%lu+: %lu\n", upperbound, buckets[numBuckets - 1]);
    free(buckets);
}
