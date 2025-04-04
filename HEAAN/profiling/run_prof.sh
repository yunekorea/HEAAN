#!/bin/zsh

oper=$1
savedir=prof_data_$1

mkdir ${savedir}

# Start profiling tools
sar -u 1 -o ${savedir}/sar_cpu_${oper}.file > /dev/null 2>&1 &
sar -r 1 -p ${savedir}/sar_memory_${oper}.file > /dev/null 2>&1 &
sar -b 1 -o ${savedir}/sar_io_${oper}.file > /dev/null 2>&1 &
sar -d --dev=nvme0n1 1 -o ${savedir}/sar_disk_${oper}.file > /dev/null 2>&1 &
sudo blktrace -d /dev/nvme0n1 -o ${savedir}/blktrace_${oper} &

# Run profheaan
./ProfilingHEAAN ${oper}

# Stop profiling tools
sudo killall blktrace
killall sar

# Process data
#blkparse -i blktrace_${oper} -o blktrace_${oper}.parsed
iowatcher -t ${savedir}/blktrace_${oper} -o ${savedir}/iowatcher_${oper}.svg

echo "Profiling complete. Check output.png and sar logs."

