#!/usr/bin/python

import subprocess
import os
import sys

"""
This script forces the cores named on the command line offline. The cores
should be specified as comma-delimited ranges.
"""

def disable(coreId):
    os.system("/bin/echo 0 > /sys/devices/system/cpu/cpu{}/online".format(coreId))
    print "Disabled Core {}.".format(coreId)

def disableRange(coreRange):
    if '-' in coreRange:
        start,end = (int(x) for x in coreRange.split('-'))
        for i in xrange(start,end+1):
            disable(i)
    else: disable(coreRange)

def main():
    if not os.geteuid() == 0:
       sys.exit('This script must be run as root!')
    if len(sys.argv) != 2:
        print "Usage: DisableCores.py <CoreRanges>"
        return
    if ',' in sys.argv[1]:
        ranges = sys.argv[1].split(",")
        for r in ranges:
            disableRange(r)
        return
    disableRange(sys.argv[1])

if __name__ == '__main__':
   main()

