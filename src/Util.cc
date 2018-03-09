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

#include "Util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <mutex>
#include <sstream>
#include <string>

#define CACHE_LINE_SIZE 64
#define gettid() syscall(SYS_gettid)

using std::string;

namespace PerfUtils {
namespace Util {
string
format(const char* format, ...) __attribute__((format(printf, 1, 2)));

string
vformat(const char* format, va_list ap) __attribute__((format(printf, 1, 0)));

/// A safe version of sprintf.
std::string
format(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    string s = vformat(format, ap);
    va_end(ap);
    return s;
}

/// A safe version of vprintf.
string
vformat(const char* format, va_list ap) {
    string s;

    // We're not really sure how big of a buffer will be necessary.
    // Try 1K, if not the return value will tell us how much is necessary.
    int bufSize = 1024;
    while (true) {
        char buf[bufSize];
        // vsnprintf trashes the va_list, so copy it first
        va_list aq;
        __va_copy(aq, ap);
        int r = vsnprintf(buf, bufSize, format, aq);
        assert(r >= 0);  // old glibc versions returned -1
        if (r < bufSize) {
            s = buf;
            break;
        }
        bufSize = r + 1;
    }

    return s;
}

/**
 * Return (potentially multi-line) string hex dump of a binary buffer in
 * 'hexdump -C' style.
 * Note that this exceeds 80 characters due to 64-bit offsets.
 */
std::string
hexDump(const void* buf, uint64_t bytes) {
    const unsigned char* cbuf = reinterpret_cast<const unsigned char*>(buf);
    uint64_t i, j;

    std::ostringstream output;
    for (i = 0; i < bytes; i += 16) {
        char offset[17];
        char hex[16][3];
        char ascii[17];

        snprintf(offset, sizeof(offset), "%016lx", i);
        offset[sizeof(offset) - 1] = '\0';

        for (j = 0; j < 16; j++) {
            if ((i + j) >= bytes) {
                snprintf(hex[j], sizeof(hex[0]), "  ");
                ascii[j] = '\0';
            } else {
                snprintf(hex[j], sizeof(hex[0]), "%02x", cbuf[i + j]);
                hex[j][sizeof(hex[0]) - 1] = '\0';
                if (isprint(static_cast<int>(cbuf[i + j])))
                    ascii[j] = cbuf[i + j];
                else
                    ascii[j] = '.';
            }
        }
        ascii[sizeof(ascii) - 1] = '\0';

        output << format(
            "%s  %s %s %s %s %s %s %s %s  %s %s %s %s %s %s %s %s  "
            "|%s|\n",
            offset, hex[0], hex[1], hex[2], hex[3], hex[4], hex[5], hex[6],
            hex[7], hex[8], hex[9], hex[10], hex[11], hex[12], hex[13], hex[14],
            hex[15], ascii);
    }
    return output.str();
}

/**
 * Split a string by the given single-character delimiter, and append the
 * split pieces into elems.
 */
void
split(const std::string& s, char delim, std::vector<std::string>& elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

/**
 * Split a string by the given single-character delimiter, and return the split
 * pieces into an std::vector<std::string>.
 */
std::vector<std::string>
split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

/**
 * Take a string containing comma-separated ranges of integers and return an
 * std::vector containing the numbers in each range.
 */
std::vector<int>
parseRanges(const char* coreDesc) {
    std::vector<int> cores;
    std::vector<std::string> ranges = split(coreDesc, ',');
    for (size_t i = 0; i < ranges.size(); i++) {
        if (ranges[i].find("-") == std::string::npos) {
            cores.push_back(atoi(ranges[i].c_str()));
        } else {
            auto bounds = split(ranges[i], '-');
            for (int k = atoi(bounds[0].c_str()); k <= atoi(bounds[1].c_str());
                 k++)
                cores.push_back(k);
        }
    }
    return cores;
}

/**
 * Return all the cores that the current process has access to.
 */
std::vector<int>
getAllUseableCores() {
    FILE* fp;
    char path[1024];

    fp = popen("cat /sys/fs/cgroup/cpuset$(cat /proc/self/cpuset)/cpuset.cpus",
               "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        exit(1);
    }
    if (fgets(path, sizeof(path) - 1, fp) == NULL) {
        fprintf(stderr, "No cores found!\n");
        return std::vector<int>();
    }
    return parseRanges(path);
}

/**
 * Allocate a block of memory aligned at the beginning of a cache line.
 *
 * \param size
 *     The amount of memory to allocate.
 */
void*
cacheAlignAlloc(size_t size) {
    void* temp;
    int result = posix_memalign(&temp, CACHE_LINE_SIZE, size);
    if (result == 0) {
        assert((reinterpret_cast<uint64_t>(temp) & (CACHE_LINE_SIZE - 1)) == 0);
        return temp;
    }
    return NULL;
}

/**
 * Returns the CPU ID of the physical core corresponding to coreId.
 * The physical core is defined as the lowest-numbered sibling of coreId.
 *
 * \param coreId
 *     The coreId whose hypertwin's ID will be returned.
 */
int
getPhysicalCore(int coreId) {
    // This file contains the siblings of core coreId.
    std::string siblingFilePath = "/sys/devices/system/cpu/cpu" +
                                  std::to_string(coreId) +
                                  "/topology/thread_siblings_list";
    FILE* siblingFile = fopen(siblingFilePath.c_str(), "r");
    int physicalCoreId;
    // The first cpuid in the file is always that of the physical core
    fscanf(siblingFile, "%d", &physicalCoreId);
    return physicalCoreId;
}

}  // namespace Util
}  // namespace PerfUtils
