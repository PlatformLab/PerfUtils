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

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ContainerEq;
using ::testing::ElementsAre;
using ::testing::Eq;

TEST(UtilTest, readIntegers) {
    // Test an empty file.
    int fd = open("/dev/null", O_RDWR);
    std::vector<int> retVal = PerfUtils::Util::readIntegers(fd, '\n');
    EXPECT_EQ(0, retVal.size());
    close(fd);

    // Create a file for testing
    char* filename = strdup("/tmp/readIntegers_XXXXXX");
    fd = mkstemp(filename);
    for (int i = 0; i < 1024; i++) {
        char numBuf[10];
        snprintf(numBuf, sizeof(numBuf), "%d\n", i);
        write(fd, numBuf, strlen(numBuf));
    }

    // Test the temporary file
    retVal = PerfUtils::Util::readIntegers(fd, '\n');
    EXPECT_EQ(1024, retVal.size());

    for (int i = 0; i < retVal.size(); i++) {
        EXPECT_EQ(i, retVal[i]);
    }
    close(fd);
}

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
    EXPECT_THAT(result, ElementsAre(1, 2, 3, 5, 6, 7));
}

TEST(UtilTest, getHyperTwin) {
    int hyperZero = PerfUtils::Util::getHyperTwin(0);
    int physicalCore = PerfUtils::Util::getPhysicalCore(hyperZero);
    EXPECT_THAT(physicalCore, Eq(0));
}

TEST(UtilTest, containerToUnorderedSet) {
    std::vector<int> vec{1, 2, 3};
    std::unordered_set<int> set = PerfUtils::Util::containerToUnorderedSet(vec);
    EXPECT_THAT(set, ContainerEq(std::unordered_set<int>{1, 2, 3}));

    std::vector<int> deq{4, 5, 6};
    set = PerfUtils::Util::containerToUnorderedSet(deq);
    EXPECT_THAT(set, ContainerEq(std::unordered_set<int>{4, 5, 6}));
}
