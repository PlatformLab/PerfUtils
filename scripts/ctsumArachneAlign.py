#!/usr/bin/env python

# Copyright (c) 2018 Stanford University
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

"""
This program reads timetrace log files and Arachne core estimation log file, 
then generate a combined and aligned core map + estimation stats dump.
"""

from __future__ import division, print_function
from glob import glob
from optparse import OptionParser
from argparse import ArgumentParser
import math
import os
import re
import string
import sys
import fileinput

# This variable collects the core ids for each thread. It is a dictionary
# that maps from thread names to a list of their all cpu core ids. The last
# element in the list is their current cpu core id. 
threadCoreIds = {}

# This variable is an invert dictionary of threadCoreIds, which maps from cpu
# core ids to a list of their current running thread names
coreIdThreads = {}

# This variable is the number of physical cores (hypertwins) in the system
numCores = 0

def readOneDump(f):
    dumpString = ''
    startCycle = 0
    for line in f:
        dumpString += line
        # Check whether it is the end
        match = re.match('END ESTIMATION STATS DUMP', line)
        if match:
            break
        match = re.match('TimeInCycles = ([0-9.]+),', line)
        if match:
            startCycle = float(match.group(1))
    return startCycle, dumpString

def scan(f, flog):
    """
    Scan the log file given by 'f' (handle for an open file) and collect
    information from the trace records as described by the arguments.
    And we print only when worker threads have changed cores in this case.
    """

    print('Number of physical cores: {:d}'.format(numCores))
    
    # Setup initial core map, the list for each core is empty
    for id in xrange(0, numCores):
        coreIdThreads[id] = []

    # Print the header line
    hypertwin = numCores // 2
    print('Total Time (ns)  Delta Time (ns) ', end='')
    for id in xrange(0, hypertwin):
        print('  {:>8}|{:<8}'.format(int(id), int(id + hypertwin)), end='')
    print('')

    lastPrintTime = 0
    startCycle = 0 # coretrace start cycle, should be earlier than estimation.
    cyclesPerNanoSec = 1.0 # cycles per nano second
    # pre-read one chunk from Arachne estimation log.
    flogTimeCycle, flogString = readOneDump(flog)
    # If false, means we have not encountered the cleanup point and we should
    # print nothing.
    cleanedUpRampUp = False
    for line in f:
        # First, find the start cycles
        match = re.match('START_CYCLES ([0-9]+)', line)
        if match:
            startCycle = float(match.group(1))
            continue
        match = re.match('CYCLES_PER_SECOND ([0-9.]+)', line)
        if match:
            cyclesPerNanoSec = float(match.group(1)) / 1e9
            continue

        # Try to match the coretrace
        match = re.match('.* [-]+CLEANUP CORETRACE[-]+', line)
        if match:
            # We don't want the rampup phase and ended phase. Just keep the
            # benchmark running part.
            cleanedUpRampUp = not cleanedUpRampUp
            continue

        match = re.match(' *([0-9.]+) ns \(\+ *([0-9.]+) ns\): (.*)', line)
        if not match:
            continue
        thisEventTime = float(match.group(1))
        thisEventInterval = float(match.group(2))
        thisEvent = match.group(3)
        submatch = re.match('\[(.*)\] cpuid: ([0-9]+)', thisEvent)
        if not submatch:
            continue
        threadName = submatch.group(1)
        thisCoreId = int(submatch.group(2))

        # Only update the coremap if we have passed the cleanup point.
        if cleanedUpRampUp:
            prevCoreId = -1
            if threadName not in threadCoreIds:
                threadCoreIds[threadName] = []
            else:
                prevCoreId = threadCoreIds[threadName][-1]

            # Clear the data on the previous core
            if prevCoreId >= 0 and threadName in coreIdThreads[prevCoreId]:
                coreIdThreads[prevCoreId].remove(threadName)
            threadCoreIds[threadName].append(thisCoreId)

            coreIdThreads[thisCoreId].append(threadName)

        # Print the current core map only if a worker thread changed the core.
        isWorker = re.match('(w[0-9]+)', threadName)
        if isWorker:
            # Check whether we should print the estimation log.
            # Because flogTimeCycle is raw cycle, while thisEventTime is a
            # relative cycle. We need to use startCycle to calculate the
            # relative estimation log cycle.
            thisDumpTime = (flogTimeCycle - startCycle) / cyclesPerNanoSec
            while  thisDumpTime <= thisEventTime:
                # if flogString is empty, it means we have reached the end of
                # estimation log file.
                if not flogString:
                    break
                # Don't print estimation logs if no coremap has been printed.
                if cleanedUpRampUp and (lastPrintTime > 0):
                    print('{:>15.2f}  {:>15.2f} '.format(thisDumpTime, thisDumpTime - lastPrintTime))
                    print(flogString)
                    lastPrintTime = thisDumpTime

                flogTimeCycle, flogString = readOneDump(flog)
                thisDumpTime = (flogTimeCycle - startCycle) / cyclesPerNanoSec

            if cleanedUpRampUp:
                printCoreMap(thisEventTime, thisEventTime - lastPrintTime)
                lastPrintTime = thisEventTime


def printCoreMap(currTime, deltaTime):
    """
    Print the current time, interval time (in ns), and the current core map
    (core id => thread names) based on coreIdThreads. Print in the group of
    hypertwins. Print multiple lines if there are multiple threads on one core.
    """

    print('{:>15.2f}  {:>15.2f} '.format(currTime, deltaTime), end='')
    hypertwin = numCores // 2

    # numEntries is the number of lines we should print
    numEntries = max(len(coreIdThreads[id]) for id in coreIdThreads)

    for i in xrange(0, numEntries):
        if i > 0:
            # Print the preceeding space
            print('{:>15}  {:>15} '.format('', ''), end='')
        for id in xrange(0, hypertwin):
            print('  ', end='')
            if len(coreIdThreads[id]) < i + 1:
                print('{:>8}'.format(''), end='')
            else:
                threadName = coreIdThreads[id][i]
                print('{:>8s}'.format(threadName), end='')

            print('|', end='')

            if len(coreIdThreads[id+hypertwin]) < i + 1:
                print('{:<8}'.format(''), end='')
            else:
                threadName = coreIdThreads[id+hypertwin][i]
                print('{:<8s}'.format(threadName), end='')
        print('')
    print('\n')

def parseArgs():
    parser = ArgumentParser(description=
             'Read one log file and summarize the core map information.',
             usage='%%prog [arguments] file')
    parser.add_argument('--numcores', '-n', type=int, required=False,
        default=16, dest='numCores',
        help='The number of physical cores in the system, including hypertwins')
    parser.add_argument('--input', '-i', type=str, required=True,
        dest='fileName',
        help='Input coretrace log file name.')
    parser.add_argument('--estimation-log', '-l', type=str, required=True,
        dest='estimationLog',
        help='Input Arachne estimation log file name.')
    return parser.parse_args()


# The main entraince of the script
def main(args):
    global numCores
    numCores = args.numCores
    fileName = args.fileName
    estimationLog = args.estimationLog
    scan(open(fileName), open(estimationLog));


if __name__ == '__main__':
    args = parseArgs()
    if args.fileName is None:
        print("No log files given")
        sys.exit(1)
    main(args)
