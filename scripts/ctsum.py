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
This program reads timetrace log files and generates the core maps whenever a
thread changes the core
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

def scan(f):
    """
    Scan the log file given by 'f' (handle for an open file) and collect
    information from the trace records as described by the arguments.
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

    for line in f:
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

        # Print the current core map
        printCoreMap(thisEventTime, thisEventInterval)


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
        help='Input log file name.')
    return parser.parse_args()


# The main entraince of the script
def main(args):
    global numCores
    numCores = args.numCores
    fileName = args.fileName
    scan(open(fileName));


if __name__ == '__main__':
    args = parseArgs()
    if args.fileName is None:
        print("No log files given")
        sys.exit(1)
    main(args)
