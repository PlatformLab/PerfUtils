/* Copyright (c) 2014 Stanford University
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
    void record(const char* message, uint64_t timestamp = Cycles::rdtsc());
    void print();
    std::string getTrace();
    static TimeTrace* getGlobalInstance();

  private:
    void printInternal(std::string* s);

    /**
     * This structure holds one entry in the TimeTrace.
     */
    struct Event {
      uint64_t timestamp;        // Time when a particular event occurred.
      const char* message;       // Static string describing the event.
                                 // NULL means that this entry is unused.
    };

    // Total number of events that we can retain any given time.
    static const int BUFFER_SIZE = 10000;

    // Holds information from the most recent calls to the record method.
    Event events[BUFFER_SIZE];

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

