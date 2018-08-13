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
    if (*(const uint64_t*)a == *(const uint64_t*)b)
        return 0;
    return *(const uint64_t*)a < *(const uint64_t*)b ? -1 : 1;
}

Statistics
computeStatistics(uint64_t* rawdata, size_t count) {
    Statistics stats;
    if (count == 0)
        return stats;
    qsort(rawdata, count, sizeof(uint64_t), compare);
    uint64_t sum = 0;
    for (size_t i = 0; i < count; i++)
        sum += rawdata[i];
    stats.count = count;
    stats.average = sum / count;
    stats.stddev = 0;
    for (size_t i = 0; i < count; i++) {
        uint64_t diff = rawdata[i] > stats.average ? rawdata[i] - stats.average
            : stats.average - rawdata[i];
        stats.stddev += diff * diff;
    }
    stats.stddev /= count;
    stats.stddev = static_cast<uint64_t>(sqrt(stats.stddev));
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

Statistics
transformStatistics(Statistics stats, uint64_t (*function)(uint64_t)) {
    Statistics result;
    result.count = stats.count;
    result.average = function(stats.average);
    result.stddev = function(stats.stddev);
    result.min = function(stats.min);
    result.median = function(stats.median);
    result.P10 = function(stats.P10);
    result.P20 = function(stats.P20);
    result.P30 = function(stats.P30);
    result.P40 = function(stats.P40);
    result.P50 = function(stats.P50);
    result.P60 = function(stats.P60);
    result.P70 = function(stats.P70);
    result.P80 = function(stats.P80);
    result.P90 = function(stats.P90);
    result.P99 = function(stats.P99);
    result.P999 = function(stats.P999);
    result.P9999 = function(stats.P9999);
    result.max = function(stats.max);
	return result;
}

void
printStatistics(Statistics stats, const char* label) {
	static bool headerPrinted = false;
	if (!headerPrinted) {
		puts("Benchmark,Count,Avg,Stddev,Median,Min,99%,"
				"99.9%,99.99%,Max");
		headerPrinted = true;
	}
	printf("%s,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu\n", label, stats.count, stats.average,
			stats.stddev, stats.median, stats.min, stats.P99, stats.P999, stats.P9999,
			stats.max);
}

void
printStatistics(const char* label, uint64_t* rawdata, size_t count,
                const char* datadir) {
    Statistics stats = computeStatistics(rawdata, count);
	printStatistics(stats, label);

    // Dump the data out
    if (datadir != NULL) {
        char buf[1024];
        snprintf(buf, sizeof(buf), "%s/%s", datadir, label);
        ensureParents(buf);
        FILE* fp = fopen(buf, "w");
        for (size_t i = 0; i < count; i++)
            fprintf(fp, "%lu\n", rawdata[i]);
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
