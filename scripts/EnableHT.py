#!/usr/bin/python

import os
import sys
from subprocess import Popen, PIPE, STDOUT, check_output

"""
This thread enables hyperthreading by iterating over all configured cores and
taking them online.
"""

def main():
    if not os.geteuid() == 0:
       sys.exit('This script must be run as root!')

    totalCores = int(check_output("getconf _NPROCESSORS_CONF",
        shell=True))

    for i in xrange(totalCores):
        try:
          enable_cmd = Popen(
              ["/bin/echo 1 > /sys/devices/system/cpu/cpu{}/online"
              .format(i)], shell=True, stdout=PIPE, stderr=STDOUT)
          stdout, _ = enable_cmd.communicate()

        except:
          pass


if __name__ == '__main__':
   main()

