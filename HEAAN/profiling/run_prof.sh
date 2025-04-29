#!/bin/zsh


MEM_LIMIT=""
human_limit=""
oper=""

# List of allowed operations
allowed_opers=("enc" "cipadd" "cipmult" "decadd" "decmult")

# Argument parsing
while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
    -l|--limit)
      human_limit="$2"
      if [[ -z "$human_limit" ]]; then
        echo "Error: No value provided for $key"
        exit 1
      fi
      # Parse human-readable sizes
      if [[ "$human_limit" =~ ^[0-9]+[MmGg]$ ]]; then
        case "$human_limit" in
          *M|*m) MEM_LIMIT=$(( ${human_limit%[Mm]} * 1024 * 1024 )) ;;
          *G|*g) MEM_LIMIT=$(( ${human_limit%[Gg]} * 1024 * 1024 * 1024 )) ;;
        esac
      else
        echo "Invalid limit format: use like 512M or 2G"
        exit 1
      fi
      shift 2
      ;;
    -*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      if [[ -z "$oper" ]]; then
        oper="$1"
      else
        echo "Error: Unexpected extra argument '$1'"
        exit 1
      fi
      shift
      ;;
  esac
done

# Check if operation is valid
if [[ ! " ${allowed_opers[@]} " =~ " ${oper} " ]]; then
  echo "Error: Invalid operation '$oper'"
  echo "Allowed operations: ${allowed_opers[@]}"
  exit 1
fi

echo "Selected operation: $oper"
if [[ -n "$MEM_LIMIT" ]]; then
  echo "Memory limit: ${MEM_LIMIT} bytes"
else
  echo "No memory limit will be applied."
fi

#make directory for profiled data
dir=prof_data_$oper
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

if [[ -n "$MEM_LIMIT" ]]; then
  # Create swap file & cgroup for memory limitation
  sudo fallocate -l 128G /mnt/ssd/swapfile
  sudo chmod 600 /mnt/ssd/swapfile
  sudo mkswap /mnt/ssd/swapfile
  sudo swapon /mnt/ssd/swapfile -p 200
  sudo cgcreate -g memory:testlimit
  echo ${MEM_LIMIT} | sudo tee /sys/fs/cgroup/testlimit/memory.max

  sudo perf record --call-graph dwarf -g -- ./ProfilingHEAAN ${oper} &
  HEAAN_PID=$!
  echo ${HEAAN_PID} | sudo tee /sys/fs/cgroup/testlimit/cgroup.procs
  wait ${HEAAN_PID}
  # Remove swap file & cgroup
  sudo swapoff /mnt/ssd/swapfile
  sudo rm /mnt/ssd/swapfile
  sudo cgdelete -g memory:testlimit
else
  sudo perf record --call-graph dwarf -g ./ProfilingHEAAN ${oper}
fi

# Stop profiling tools
sudo killall blktrace
killall sar

sleep 5

# Process data

mv perf.data ${savedir}/
iowatcher -t ${savedir}/blktrace_${oper} -o ${savedir}/iowatcher_${oper}.svg
sleep 5
blkparse -i ${savedir}/blktrace_${oper} -d ${savedir}/blkparse_${oper}_dump.bin

cd ${savedir}
btt -i ${savedir}/blkparse_${oper}_dump.bin -o ${savedir}/btt_${oper}
sudo perf script | ~/FlameGraph/stackcollapse-perf.pl | ~/FlameGraph/flamegraph.pl > FlameGraph_${oper}.svg
sudo rm -f perf.data
cd ../

# make output text file from sar binary output
sar -u -f ${savedir}/sar_cpu_${oper}.file > ${savedir}/sar_cpu_${oper}.out
sar -r -f ${savedir}/sar_memory_${oper}.file > ${savedir}/sar_memory_${oper}.out
sar -b -f ${savedir}/sar_io_${oper}.file > ${savedir}/sar_io_${oper}.out
sar -d --dev=nvme0n1 -f ${savedir}/sar_disk_${oper}.file > ${savedir}/sar_disk_${oper}.out
sar -B -f ${savedir}/sar_paging_${oper}.file > ${savedir}/sar_paging_${oper}.out 

echo "Profiling complete. Check output.svg and sar logs."

