#!/usr/bin/env python

# Copyright (c) 2017 Stanford University
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
This program reads multiple log file which must have been generated from the
same machine and outputs a combined log file.
"""

from __future__ import division, print_function
from glob import glob
from docopt import docopt
from collections import namedtuple
import math
import os
import re
import string
import sys

Event = namedtuple('Event', ['timestamp', 'message'])
Trace = namedtuple('Trace', ['trace', 'cyclesPerSecond', 'startCycles'])
def parseTrace(f):
  traceOutput = []
  cyclesPerSecond = 0
  startCycles = 0
  with open(f) as trace:
      for line in trace:
        if 'CYCLES_PER_SECOND' in line:
            cyclesPerSecond = float(line.strip().split()[1])
            continue
        if 'START_CYCLES' in line:
            startCycles = int(line.strip().split()[1])
            continue
        match = re.match(' *([0-9.]+) ns \(\+ *([0-9.]+) ns\): (.*)', line)
        if not match: continue
        thisEventTime = float(match.group(1))
        thisEvent = match.group(3)
        traceOutput.append(Event(thisEventTime, thisEvent))
  return Trace(traceOutput, cyclesPerSecond, startCycles)


doc = r"""
Usage: ./ttmerge.py [-h] [-k] [<input> ...]

    -h,--help                  show this
    -k,--keepOldEvents         keep old events rather than truncating them
"""
def main():
  options = docopt(doc)
  rawTraces = []
  for i in options["<input>"]:
    rawTraces.append(parseTrace(i))

  if not rawTraces:
    print(doc)
    return

  # Adjust traces using the same estimate of cyclesPerSecond, so that we can
  # line them up.
  traces = []
  cyclesPerSecond = min(x.cyclesPerSecond for x in rawTraces)
  for rawTrace in rawTraces:
    delta = rawTrace.startCycles / cyclesPerSecond * 1e9
    traces.append(
      [Event(x.timestamp * rawTrace.cyclesPerSecond / cyclesPerSecond + delta,
             x.message) for x in rawTrace.trace])


  # The merge algorithm is similar to the merge used within a single TimeTrace
  # between threads. In particular, it starts at the most recent of the oldest
  # times among the input traces.

  # Decide on the time of the first event to be included in the output.
  if not options['--keepOldEvents']:
    # This is most recent of the oldest times in all the traces (an empty
    # trace has an "oldest time" of 0). The idea here is to make sure
    # that there's no missing data in what we print (if trace A goes back
    # farther than trace B, skip the older events in trace A, since there
    # might have been related events that were once in trace B but have since
    # been overwritten).
    startTime = 0;
    for trace in traces:
      if trace[0].timestamp > startTime:
        startTime = trace[0].timestamp
  else:
    # This is the oldest time across all the traces; combined trace will
    # include data from threads which logged only when application first
    # started.
    startTime = sys.maxint
    for trace in traces:
      if trace[0].timestamp < startTime:
        startTime = trace[0].timestamp

  # Remove all events before the starting time.
  for trace in traces:
    while len(trace) > 0 and trace[0].timestamp < startTime:
      trace.pop(0)

  # Make it possible to do further merges with this trace if we are interested.
  print("CYCLES_PER_SECOND %f" % cyclesPerSecond)
  print("START_CYCLES %d" % int(startTime / 1e9 * cyclesPerSecond))

  # Each iteration through this loop processes one event (the one with
  # the earliest timestamp).
  prevTime = startTime
  while True:
    chosenTrace = None
    earliestTime = sys.float_info.max;
    for trace in traces:
      # At least one empty trace means we have exhausted the earliest of the
      # newest events in the trace.
      if len(trace) == 0:
        chosenTrace = None
        break
      if trace[0].timestamp < earliestTime:
        earliestTime = trace[0].timestamp
        chosenTrace = trace
    if not chosenTrace:
      break
    event = chosenTrace.pop(0)
    print('%8.1f ns (+%6.1f ns): %s' % (event.timestamp - startTime, \
        event.timestamp - prevTime, event.message))
    prevTime = event.timestamp

if __name__ == "__main__":
  main()
