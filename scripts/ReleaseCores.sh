#!/bin/bash

# Usage: ./ReleaseCores.sh

if [[ $(id -u) -ne 0 ]]; then
    echo "You must be root to run this script!"
    exit
fi


# Restore all processes to the root cpuset
cd /sys/fs/cgroup/cpuset
for i in $(ps aux | awk '{print $2}'); do
    echo $i > cgroup.procs 2> /dev/null
done

# Remove all the cpusets
rmdir /sys/fs/cgroup/cpuset/* 2> /dev/null
ls /sys/fs/cgroup/cpuset/
