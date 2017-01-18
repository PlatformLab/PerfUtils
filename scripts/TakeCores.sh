#!/bin/bash

# Assign the cores specified in <MyCores> to the process with pid equal to
# <my_pid>, and relevant all other processes to the cores in <OtherCores>.
# Core format is a comma-separated list of ranges. e.g. 1-3,5
# Usage: ./TakeCores.sh <MyCores> <OtherCores> <my_pid>

if [[ $(id -u) -ne 0 ]]; then
    echo "You must be root to run this script!"
    exit
fi

if [[ "$#" -lt 3 ]]; then
    echo "Usage: ./TakeCores.sh <MyCores> <OtherCores> <my_pid>"
    exit
fi

cd /sys/fs/cgroup/cpuset
mkdir -p AllOthers
echo "$2" > AllOthers/cpuset.cpus

# This line may only be nec. on RC cluster machines.
cat cpuset.mems > AllOthers/cpuset.mems

# Move all other processes away
for i in $(cat cgroup.procs ); do
    echo $i > AllOthers/cgroup.procs 2> /dev/null;
done

# Create a private group for my process
mkdir -p CurrentProcess
echo "$1" > CurrentProcess/cpuset.cpus
cat cpuset.mems > CurrentProcess/cpuset.mems

# Put my process into it.
echo "$3" > CurrentProcess/cgroup.procs
