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
from optparse import OptionParser
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

def main():
  rawTraces = []
  for i in sys.argv[1:]:
    rawTraces.append(parseTrace(i))

  if not rawTraces: return

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
  startTime = 0;
  for trace in traces:
    if trace[0].timestamp > startTime:
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
      if len(trace) == 0: continue
      if trace[0].timestamp < earliestTime:
        earliestTime = trace[0].timestamp
        chosenTrace = trace
    if not chosenTrace:
      break
    event = chosenTrace.pop(0)
    print('%8.1f ns (+%6.1f ns): %s' % (event.timestamp - startTime, \
        event.timestamp - prevTime, event.message))
    if not chosenTrace:
      break
    prevTime = event.timestamp

if __name__ == "__main__":
  main()
