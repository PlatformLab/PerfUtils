/* Copyright (c) 2018 Stanford University
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

#include <errno.h>
#include <stdio.h>

#include "TimeTrace.h"
using PerfUtils::TimeTrace;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function is the wrapper for TimeTrace::setOutputFileName
 */
void
timetrace_set_output_filename(const char* filename) {
    TimeTrace::setOutputFileName(filename);
}

/**
 * This function is the wrapper for TimeTrace::print
 */
void
timetrace_print() {
    TimeTrace::print();
}

/**
 * This function is the wrapper for TimeTrace::record
 *
 * Since C does not support default value, caller always needs to pass arg0-3.
 * Also, we cannot separate definition and declaration of inline function, so
 * this function cannot be inline function.
 */
void
timetrace_record(const char* format, uint32_t arg0, uint32_t arg1,
                 uint32_t arg2, uint32_t arg3) {
    TimeTrace::record(format, arg0, arg1, arg2, arg3);
}

/**
 * This function is used to set TimeTrace::keepOldEvents
 */
void
timetrace_set_keepoldevents(bool keep) {
    TimeTrace::keepOldEvents = keep;
}

#ifdef __cplusplus
}
#endif
