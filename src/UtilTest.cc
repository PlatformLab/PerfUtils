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

#include "Util.h"

#include <string.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAre;

TEST(UtilTest, fileGetContents) {
    // Test an empty file.
    char* result = PerfUtils::Util::fileGetContents("/dev/null");
    EXPECT_EQ(0, strlen(result));
    delete[] result;

    // Test a file with some content.
    char content[] = "Hello World!\n";
    FILE* f = fmemopen(content, sizeof(content), "r");
    result = PerfUtils::Util::fileGetContents(f);
    EXPECT_STREQ(content, result);
    delete[] result;
}

TEST(UtilTest, readRanges) {
    // Test an empty file.
    std::vector<int> result = PerfUtils::Util::readRanges("/dev/null");
    EXPECT_EQ(0, result.size());

    char content[] = "1-3,5-7";
    FILE* f = fmemopen(content, sizeof(content), "r");
    result = PerfUtils::Util::readRanges(f);
    ASSERT_THAT(result, ElementsAre(1, 2, 3, 5, 6, 7));
}
