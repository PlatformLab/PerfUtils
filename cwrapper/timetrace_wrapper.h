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

#ifndef WRAPPER_PERFUTIL_TIMETRACE_H
#define WRAPPER_PERFUTIL_TIMETRACE_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void timetrace_set_output_filename(const char* filename);
void timetrace_print();
/**
 * The real signature of this function is the following. The format string is
 * mandatory; remaining arguments are only necessary as specified by format
 * string.
 * void timetrace_record(const char* format, uint32_t arg0, uint32_t arg1,
 *                       uint32_t arg2, uint32_t arg3);
 */
void timetrace_record();
void timetrace_set_keepoldevents(bool keep);

#ifdef __cplusplus
}
#endif

#endif  // WRAPPER_PERFUTIL_TIMETRACE_H
