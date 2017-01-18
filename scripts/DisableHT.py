#!/usr/bin/python

import subprocess
import os
import sys

"""
This script attempts to disable hyperthreading by forcing offline all
hyperthreads on a each core except one.
"""

def disable(coreId):
    os.system("/bin/echo 0 > /sys/devices/system/cpu/cpu{}/online".format(coreId))
    print "Disabled Core {}.".format(coreId)

def main():
    if not os.geteuid() == 0:
       sys.exit('This script must be run as root!')

    totalCores = int(subprocess.check_output("getconf _NPROCESSORS_CONF",
        shell=True))
    thread_siblings = "/sys/devices/system/cpu/cpu{}/topology/thread_siblings_list"

    complete = False
    while not complete:
        complete = True
        for i in xrange(totalCores):
          try:
            with open(thread_siblings.format(i)) as sibling_list:
                siblings = sibling_list.read()
                if '-' in siblings:
                    hyperthreads = siblings.split("-")
                    disable(hyperthreads[1].strip())
                    complete = False
                elif ',' in siblings:
                    hyperthreads = siblings.split(",")
                    for h in hyperthreads[1:]:
                        disable(h.strip())
                    complete = False
          except:
            pass



if __name__ == '__main__':
   main()

