#ifndef PROFILING_H_
#define PROFILING_H_

#include "../src/HEAAN.h"
//#include "FileLoad.h"
#include "FileSave.h"
#include <string>
#include <linux/fs.h>
#include <linux/fiemap.h>

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

#endif
