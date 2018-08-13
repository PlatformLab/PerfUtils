/* Copyright (c) 2014-2017 Stanford University
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

#include "Stats.h"

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

/**
 * Simple test function for transforming statistics.
 */
uint64_t half(uint64_t n) {
	return n / 2;
}

TEST(StatsTest, allStats) {
    const int numElements = 100;
    uint64_t input[numElements];
    for (uint64_t i = 0; i < numElements; i++)
        input[i] = i;
    Statistics stats = computeStatistics(input, numElements);
    EXPECT_EQ(numElements, stats.count);
    EXPECT_EQ(0, stats.min);
    EXPECT_EQ(99, stats.max);
    EXPECT_EQ(50, stats.median);
    EXPECT_EQ(49, stats.average);
    EXPECT_EQ(28, stats.stddev);
}

TEST(StatsTest, transformStatistics) {
    const int numElements = 100;
    uint64_t input[numElements];
    for (uint64_t i = 0; i < numElements; i++)
        input[i] = i;
    Statistics stats = computeStatistics(input, numElements);
	Statistics halfStats =  transformStatistics(stats, half);

	EXPECT_EQ(stats.count, halfStats.count);
    EXPECT_EQ(half(stats.average), halfStats.average);
    EXPECT_EQ(half(stats.stddev), halfStats.stddev);
    EXPECT_EQ(half(stats.min), halfStats.min);
    EXPECT_EQ(half(stats.median), halfStats.median);
    EXPECT_EQ(half(stats.P10), halfStats.P10);
    EXPECT_EQ(half(stats.P20), halfStats.P20);
    EXPECT_EQ(half(stats.P30), halfStats.P30);
    EXPECT_EQ(half(stats.P40), halfStats.P40);
    EXPECT_EQ(half(stats.P50), halfStats.P50);
    EXPECT_EQ(half(stats.P60), halfStats.P60);
    EXPECT_EQ(half(stats.P70), halfStats.P70);
    EXPECT_EQ(half(stats.P80), halfStats.P80);
    EXPECT_EQ(half(stats.P90), halfStats.P90);
    EXPECT_EQ(half(stats.P99), halfStats.P99);
    EXPECT_EQ(half(stats.P999), halfStats.P999);
    EXPECT_EQ(half(stats.P9999), halfStats.P9999);
    EXPECT_EQ(half(stats.max), halfStats.max);
}
