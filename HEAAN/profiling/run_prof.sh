#!/bin/zsh

oper=$1
dir=prof_data_$1

#make directory for profiled data
mkdir ${dir}
savedir=$PWD/${dir}

echo ${savedir}
# Start profiling tools
sar -u 1 -o ${savedir}/sar_cpu_${oper}.file > /dev/null 2>&1 &
sar -r 1 -o ${savedir}/sar_memory_${oper}.file > /dev/null 2>&1 &
sar -b 1 -o ${savedir}/sar_io_${oper}.file > /dev/null 2>&1 &
sar -d 1 --dev=nvme0n1 -o ${savedir}/sar_disk_${oper}.file > /dev/null 2>&1 &
sar -B 1 -o ${savedir}/sar_paging_${oper}.file > /dev/null 2>&1 &
sudo blktrace -d /dev/nvme0n1 -o ${dir}/blktrace_${oper} &

# Run profiling
echo $$ | sudo tee /sys/fs/cgroup/testlimit/cgroup.procs
sudo perf record --call-graph dwarf -g ./ProfilingHEAAN ${oper}

# Stop profiling tools
sudo killall blktrace
killall sar

sleep 5

# Process data
iowatcher -t ${savedir}/blktrace_${oper} -o ${savedir}/iowatcher_${oper}.svg
sleep 5
blkparse -i ${savedir}/blktrace_${oper} -d ${savedir}/blkparse_${oper}_dump.bin

cd ${savedir}
btt -i ${savedir}/blkparse_${oper}_dump.bin -o ${savedir}/btt_${oper}
cd ../

# make output text file from sar binary output
sar -u -f ${savedir}/sar_cpu_${oper}.file > ${savedir}/sar_cpu_${oper}.out
sar -r -f ${savedir}/sar_memory_${oper}.file > ${savedir}/sar_memory_${oper}.out
sar -b -f ${savedir}/sar_io_${oper}.file > ${savedir}/sar_io_${oper}.out
sar -d --dev=nvme0n1 -f ${savedir}/sar_disk_${oper}.file > ${savedir}/sar_disk_${oper}.out
sar -B -f ${savedir}/sar_paging_${oper}.file > ${savedir}/sar_paging_${oper}.out 

echo "Profiling complete. Check output.svg and sar logs."

