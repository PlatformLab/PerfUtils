/* Copyright (c) 2014-2016 Stanford University
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

#ifndef RAMCLOUD_TIMETRACE_H
#define RAMCLOUD_TIMETRACE_H

#include <string>
#include <xmmintrin.h>

#include "Atomic.h"
#include "Cycles.h"

namespace PerfUtils {

/**
 * This class implements a circular buffer of entries, each of which
 * consists of a fine-grain timestamp and a short descriptive string.
 * It's typically used to record times at various points in an operation,
 * in order to find performance bottlenecks. It can record a trace relatively
 * efficiently, and then either return the trace either as a string or
 * print it to the system log.
 *
 * This class is thread-safe.
 */
class TimeTrace {
  public:
    TimeTrace(const char* filename = NULL);
    ~TimeTrace();
    void record(uint64_t timestamp, const char* format, uint32_t arg0 = 0,
            uint32_t arg1 = 0, uint32_t arg2 = 0, uint32_t arg3 = 0);
    void record(const char* format, uint32_t arg0 = 0, uint32_t arg1 = 0,
            uint32_t arg2 = 0, uint32_t arg3 = 0) {
        record(Cycles::rdtsc(), format, arg0, arg1, arg2, arg3);
    }
    void print();
    std::string getTrace();
    static TimeTrace* getGlobalInstance();
    void reset();

  private:

    /**
     * Prefetch the cache lines containing [object, object + numBytes) into the
     * processor's caches.
     * The best docs for this are in the Intel instruction set reference under
     * PREFETCH.
     * \param object
     *      The start of the region of memory to prefetch.
     * \param numBytes
     *      The size of the region of memory to prefetch.
     */
    static inline void
    prefetch(const void* object, uint64_t numBytes)
    {
        uint64_t offset = reinterpret_cast<uint64_t>(object) & 0x3fUL;
        const char* p = reinterpret_cast<const char*>(object) - offset;
        for (uint64_t i = 0; i < offset + numBytes; i += 64)
            _mm_prefetch(p + i, _MM_HINT_T0);
    }

    void printInternal(std::string* s);

    /**
     * This structure holds one entry in the TimeTrace.
     */
    struct Event {
      uint64_t timestamp;        // Time when a particular event occurred.
      const char* format;        // Format string describing the event.
                                 // NULL means that this entry is unused.
      uint32_t arg0;             // Argument that may be referenced by format
                                 // when printing out this event.
      uint32_t arg1;             // Argument that may be referenced by format
                                 // when printing out this event.
      uint32_t arg2;             // Argument that may be referenced by format
                                 // when printing out this event.
      uint32_t arg3;             // Argument that may be referenced by format
                                 // when printing out this event.
    };

    // Total number of events that we can retain any given time.
    static const int BUFFER_SIZE = 10000;

    // Number of events to prefetch ahead, in order to minimize cache
    // misses.
    static const int NUM_PREFETCH = 2;

    // Holds information from the most recent calls to the record method.
    // Note: prefetching will cause NUM_PREFETCH extra elements past the
    // end of the buffer, to be accessed (allocating extra space avoids
    // the cost of being cleverer during prefetching).
    Event events[BUFFER_SIZE + NUM_PREFETCH];

    // Index within events of the slot to use for the next call to the
    // record method.
    Atomic<int> nextIndex;

    // True means that printInternal is currently running, so it is not
    // safe to add more records, since that could result in inconsistent
    // output from printInternal.
    volatile bool readerActive;

    // The name of the file to write records into. If it is null, then we will
    // write to stdout
    const char* filename;

    // Global instance
    static TimeTrace* globalTrace;
};

} // namespace RAMCloud

#define TRACE(exp) TimeTrace::getGlobalInstance()->record(exp)
#define TRACED(exp, t) TimeTrace::getGlobalInstance()->record(exp, t)

#endif // RAMCLOUD_TIMETRACE_H

