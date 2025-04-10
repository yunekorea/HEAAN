#!/bin/zsh

oper=$1
savedir=prof_data_$1

mkdir ${savedir}

# Start profiling tools
sar -u 1 -o ${savedir}/sar_cpu_${oper}.file > /dev/null 2>&1 &
sar -r 1 -o ${savedir}/sar_memory_${oper}.file > /dev/null 2>&1 &
sar -b 1 -o ${savedir}/sar_io_${oper}.file > /dev/null 2>&1 &
sar -d 1 --dev=nvme0n1 -o ${savedir}/sar_disk_${oper}.file > /dev/null 2>&1 &
sar -B 1 -o ${savedir}/sar_paging_${oper}.file > /dev/null 2>&1 &
sudo blktrace -d /dev/nvme0n1 -o ${savedir}/blktrace_${oper} &

# Run profheaan
./ProfilingHEAAN ${oper}

# Stop profiling tools
sudo killall blktrace
killall sar

sleep 5

# Process data
#blkparse -i blktrace_${oper} -o blktrace_${oper}.parsed
iowatcher -t $PWD/${savedir}/blktrace_${oper} -o $PWD/${savedir}/iowatcher_${oper}.svg

sleep 5

sar -u -f ${savedir}/sar_cpu_${oper}.file > ${savedir}/sar_cpu_${oper}.out
sar -r -f ${savedir}/sar_memory_${oper}.file > ${savedir}/sar_memory_${oper}.out
sar -b -f ${savedir}/sar_io_${oper}.file > ${savedir}/sar_io_${oper}.out
sar -d --dev=nvme0n1 -f ${savedir}/sar_disk_${oper}.file > ${savedir}/sar_disk_${oper}.out
sar -B -f ${savedir}/sar_paging_${oper}.file > ${savedir}/sar_paging_${oper}.out 

echo "Profiling complete. Check output.svg and sar logs."

