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

#ifndef PERFGRAPH_UTIL_H
#define PERFGRAPH_UTIL_H

#include <time.h>
#include <stdint.h>
#include <sched.h>
#include <assert.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <asm/unistd.h>
#include <cstdio>
#include <cstdlib>

#include <string>
#include <stdexcept>

namespace PerfUtils {

/**
 * Miscellaneous methods that seem like they might be useful in several
 * different places.
 */
namespace Util {

std::string hexDump(const void *buffer, uint64_t bytes);

/* Doxygen is stupid and cannot distinguish between attributes and arguments. */
#define FORCE_INLINE __inline __attribute__((always_inline))

/**
 * A utility for function for calling rdpmc and reading Intel's performance
 * counters. Returns the value of the performance monitoring counter with
 * index specified by argumemt ecx.
 *
 * \param ecx
 *    The index of the PMC register to read the value from. The correct
 *    value of this parameter is dependent on the selected pmc.
 *
 * NB:
 *
 * 1. This function will segfault if called in userspace unless the 8th bit
 *    of the CR4 register is set.
 * 2. This function's behavior will change depending on which pmc's have been
 *    selected. The selection is done using wrmsr from inside a kernel module.
 */
static
uint64_t FORCE_INLINE
rdpmc(int ecx)
{
    unsigned int a, d;
    __asm __volatile("rdpmc" : "=a"(a), "=d"(d) : "c"(ecx));
    return ((uint64_t)a) | (((uint64_t)d) << 32);
}

/**
  * Pretty crazy syscall interface for perf_event_open
  *
  * Full docs here:
  * http://web.eece.maine.edu/~vweaver/projects/perf_events/perf_event_open.html
  */
static int FORCE_INLINE 
perf_event_open(struct perf_event_attr *hw_event, 
                pid_t pid,
                int cpu, 
                int group_fd, 
                unsigned long flags)
{
    int ret;

    ret = static_cast<int>(syscall(__NR_perf_event_open, hw_event,
            pid, cpu,
            group_fd, flags));
    return ret;
}


/**
  * Returns the thread id of the calling thread
  * As long as the thread continues to run, this id is unique across all threads
  * running ont he system so it can be used to uniquely name per-thread 
  * resources
  */
static
pid_t FORCE_INLINE
gettid()
{
    return static_cast<pid_t>(syscall( __NR_gettid ));
}

/**
  * Returns the number of processes referencing the file named by
  * fileName. Relies on the fuser and the wc commands.
  *
  * WARNING: fileName is injected into a shell command, so code injection is
  * possible. The argument to this method should not be produced by data
  * taken from an untrusted user.
  *
  * BUGGY. This seems to be flakey and is not always reporting all
  * processes referencing a file. Oh well, doesn't seem to be a problem
  * (yet..)
  */ 
static 
int getNumFileReferences(const std::string& fileName) 
    __attribute__ ((unused));

static
int getNumFileReferences(const std::string& fileName){
    const int commandMaxLength = 1000;
    char command[commandMaxLength];
    const int dataLength = 32;
    char data[dataLength];
    int rc;
    int numFileReferences;
    char* fgetRet;
    snprintf(command, 
             commandMaxLength,
             "fuser %s 2>/dev/null | wc -w",
             fileName.c_str());
    FILE * pf = popen(command, "r");
    assert(pf);
    if (!pf){
        assert(false);
        goto err;
    }
    fgetRet = fgets(data, dataLength, pf);
    if (!fgetRet){
        assert(false);
        goto err;
    }
    rc = pclose(pf);
    //TODO(bjmnbraun@gmail.com) figure out the error codes of pclose
    assert(rc == 0);
    if (!(rc == 0)){
        goto err;
    }
    numFileReferences = atoi(data);
    return numFileReferences;
err:
    fprintf(stderr, "Could not run fuser command on file handle %s\n",
    fileName.c_str());
    throw std::runtime_error("Could not get num open file handles");
}


/**
 * This function pins the currently executing thread onto the CPU Core with
 * the id given in the argument.
 *
 * \param id
 *      The id of the core to pin the caller's thread to.
 */
static FORCE_INLINE
void pinThreadToCore(int id) {
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);
        CPU_SET(id, &cpuset);
        assert(sched_setaffinity(0, sizeof(cpuset), &cpuset) == 0);
}

/**
 * Returns the cpu affinity mask of the currently executing thread. The type
 * cpu_set_t encodes information about which cores the current thread is
 * permitted to run on.
 */
static FORCE_INLINE
cpu_set_t getCpuAffinity() {
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);
    assert(sched_getaffinity(0, sizeof(cpuset), &cpuset) == 0);
    return cpuset;
}

/**
 * This function sets the allowable set of cores for the currently executing
 * thread, and is usually used to restore an older set which was read using
 * getCpuAffinity().
 *
 * \param cpuset
 *      An object of type cpu_set_t which encodes the set of cores which
 *      current thread is permitted to run on.
 */
static FORCE_INLINE
void setCpuAffinity(cpu_set_t cpuset) {
    assert(sched_setaffinity(0, sizeof(cpuset), &cpuset) == 0);
}

/**
 * This function is used to seralize machine instructions so that no
 * instructions that appear after it in the current thread can run before any
 * instructions that appear before it. 
 *
 * It is useful for putting around rdpmc instructions (to pinpoint cache
 * misses) as well as before rdtsc instructions, to prevent time pollution from
 * instructions supposed to be executing before the timer starts.
 */
static FORCE_INLINE
void serialize() {
    uint32_t eax, ebx, ecx, edx;
    __asm volatile("cpuid"
        : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
        : "a" (1U));
}


/**
  * A compiler barrier preventing compiler reordering
  */
static FORCE_INLINE
void barrier(){
    asm volatile("" : : : "memory");
}

#define PG_DIE(format_, ...) do { \
    fprintf(stderr, format_, ##__VA_ARGS__); \
    fprintf(stderr, "%s:%d\n" , __FILE__, __LINE__); \
    abort(); \
} while (0)

} // end Util

} // end RAMCloud

#undef FORCE_INLINE
#endif  // RAMCLOUD_UTIL_H
