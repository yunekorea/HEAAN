#ifndef PROFILING_H_
#define PROFILING_H_

#include "../src/HEAAN.h"
//#include "FileLoad.h"
#include "FileSave.h"
#include <string>
#include <linux/fs.h>
#include <linux/fiemap.h>

extern "C" {
    #include <libnvme.h>
    #include <unistd.h>
}

#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))

typedef struct {
    uint64_t logical_offset;
    uint64_t physical_offset;
    uint64_t length;
    uint64_t lba_start;
    uint64_t lba_count;
} extent_info_t;

typedef struct {
    char *filename;
    int extent_count;
    uint64_t total_length;
    extent_info_t *extents;
} file_layout_t;

typedef struct {
    __u8	opcode;
	__u8	flags;
	__u16	rsvd;
	__u32	nsid;
	__u32	cdw2;
	__u32	cdw3;
	__u32	cdw10;
	__u32	cdw11;
	__u32	cdw12;
	__u32	cdw13;
	__u32	cdw14;
	__u32	cdw15;
	__u32	data_len;
	void*	data;
	__u32	metadata_len;
	void*   metadata;
	__u32	timeout_ms;
	__u64	result;
} ndp_passthru_cmd;

#endif
