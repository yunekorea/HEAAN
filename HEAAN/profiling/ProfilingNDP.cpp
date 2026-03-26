#include "ProfilingNDP.h"

using namespace std;
using namespace NTL;
using namespace heaan;


std::complex<double>* loadDouble(const std::string FileName);
void* data_buffer_alloc(size_t len);
file_layout_t* get_file_layout(const char *filename);
void free_file_layout(file_layout_t *layout);

int main(int argc, char **argv) {
  /*
  * Basic Parameters are in src/Params.h
  * If you want to use another parameter, you need to change src/Params.h file and re-complie this library.
  */
  
  std::string SFileName0 = "randint_cipher_1024_0_";
  std::string SFileName1 = "randint_cipher_1024_1_";
  std::string SFileNameAdd = "randint_cipher_1024_Add_";
  std::string WorkingDir = "/mnt/nvmf";
  int nvme_fd = nvme_open("nvme1n1");
  if(nvme_fd == -1) {
    cout << "Opening the file descriptor has failed." << endl;
    return -1;
  }

  // Parameters //
  int iter = 1024;
  //int iter = 10;
  std::string oper = string(argv[1]);
  cout << "iterator setup done" << endl;
  cout << "Selected Operation : " << oper << endl;
  // Construct and Generate Public Keys //

  
  // Encrypt to ciphertexts //

  // Load/add ciphertexts //
  if(oper == "cipadd"){
    std::string temp;
    std::string input_0_path;
    std::string input_1_path;
    std::string target_path;
    for(int i = 0; i < iter; i++) { 
      cout << "Progress : " << i << "/" << iter-1 << "\n";
      
      input_0_path = WorkingDir + "/1GB_ciphertexts/" + SFileName0 + std::to_string(i) + ".cip";
      //cout << input_0_path << endl;
      
      input_1_path = WorkingDir + "/1GB_ciphertexts/" + SFileName1 + std::to_string(i) + ".cip";
      //cout << input_1_path << endl;
      
      target_path = WorkingDir + "/1GB_ciphertexts/" + SFileNameAdd + std::to_string(i) + ".cip";
      //cout << target_path << endl;

      // create metadata for ioctl
      ndp_passthru_cmd cipadd_cmd = {
        .opcode = 0xe0,
        .flags = 0,
        .rsvd = 0,
        .nsid = 1,
        .cdw2 = 0,
        .cdw3 = 0,
        .cdw10 = 0,
        .cdw11 = 0,
        .cdw12 = 0,
        .cdw13 = 0,
        .cdw14 = 0,
        .cdw15 = 0,
        .data_len = 4096,
        .data = NULL,
        .metadata_len = 0,
        .metadata = NULL,
        .timeout_ms = 60000,
        .result = 0,
      };

      file_layout_t *input_0_layout = get_file_layout(input_0_path.c_str());
		if(!input_0_layout) {
			fprintf(stderr, "Failed to get file layout\n");
			return 1;
		}
		cipadd_cmd.cdw11 = (__u32)input_0_layout->extent_count;
		uint64_t cip_size = input_0_layout->total_length;
		
		fprintf(stdout, "CipSize: %ld\n", cip_size);

		file_layout_t *input_1_layout = get_file_layout(input_1_path.c_str());
		if(!input_1_layout) {
			fprintf(stderr, "Failed to get file layout\n");
			return 1;
		}
		cipadd_cmd.cdw12 = (__u32)input_1_layout->extent_count;

		int target_fd = open(target_path.c_str(), O_RDWR | O_CREAT, 0644);

		if (fallocate(target_fd, 0, 0, cip_size) != 0) {
			perror("fallocate failed");
			return 1;
		}
		file_layout_t *target_layout = get_file_layout(target_path.c_str());
		if(!target_layout) {
			fprintf(stderr, "Failed to get file layout\n");
			return 1;
		}
		cipadd_cmd.cdw13 = (__u32)target_layout->extent_count;
		
		
		cipadd_cmd.data_len = 4096;

      void* data_buffer = data_buffer_alloc(cipadd_cmd.data_len);
      memset(data_buffer, 0, cipadd_cmd.data_len);

      uint64_t* u64data = (uint64_t*)data_buffer;
		uint32_t bufnum = 0;
		u64data[bufnum++] = input_0_layout->start_offset;
		for(uint32_t i = 0; i < input_0_layout->extent_count; i++) {
			extent_info_t *ext = &input_0_layout->extents[i];
			u64data[bufnum++] = (uint64_t)ext->lba_start;
			u64data[bufnum++] = (uint64_t)ext->lba_count;
		}
		u64data[bufnum++] = input_1_layout->start_offset;
		for(uint32_t i = 0; i < input_1_layout->extent_count; i++) {
			extent_info_t *ext = &input_1_layout->extents[i];
			u64data[bufnum++] = (uint64_t)ext->lba_start;
			u64data[bufnum++] = (uint64_t)ext->lba_count;
		}
		u64data[bufnum++] = target_layout->start_offset;
		for(uint32_t i = 0; i < target_layout->extent_count; i++) {
			extent_info_t *ext = &target_layout->extents[i];
			u64data[bufnum++] = (uint64_t)ext->lba_start;
			u64data[bufnum++] = (uint64_t)ext->lba_count;
		}

		free_file_layout(input_0_layout);
		free_file_layout(input_1_layout);
		free_file_layout(target_layout);
      
      cipadd_cmd.data = data_buffer;

      int result = nvme_io_passthru64(nvme_fd, cipadd_cmd.opcode, cipadd_cmd.flags,
                      cipadd_cmd.rsvd, cipadd_cmd.nsid, cipadd_cmd.cdw2, cipadd_cmd.cdw3,
                      cipadd_cmd.cdw10, cipadd_cmd.cdw11, cipadd_cmd.cdw12, cipadd_cmd.cdw13,
                      cipadd_cmd.cdw14, cipadd_cmd.cdw15, cipadd_cmd.data_len, cipadd_cmd.data,
                      cipadd_cmd.metadata_len, cipadd_cmd.metadata, cipadd_cmd.timeout_ms, 
                      &cipadd_cmd.result);
    
    }
    cout << endl;
  }
  
  return 0;

}

std::complex<double>* loadDouble(const std::string FileName) {
    
    std::ifstream inFile(FileName);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for loading doubles.");
    }
    //complex<double>* loaded = new complex<double>[n];
    //std::vector<std::complex<double>> loadedVector;
    std::complex<double>* loadedArray = new std::complex<double>[1024];
    int i = 0;
    complex<double> number;
    while (inFile >> number) {
      // Create a complex number with the integer as the real part and 0 as the imaginary part
      //std::complex<double> complexNumber(number, 0);
      complex<double> complexNumber = number;

      // Add the complex number to the array
      loadedArray[i] = complexNumber;
      i++;
      //loadedVector.push_back(complexNumber);
    }
    
  inFile.close();
  return loadedArray;
  //return &loadedVector[0];
  
  return 0;
}

void* data_buffer_alloc(size_t len) {
  void *p;
  len = ROUND_UP(len, 0x1000);
  if (posix_memalign(&p, getpagesize(), len))
		return NULL;

	memset(p, 0, len);
	return p;
}

file_layout_t* get_file_layout(const char *filename) {
    int fd;
    struct fiemap *fiemap;
    file_layout_t *layout;

    // file_layout 구조체 할당
    layout = (file_layout_t*)malloc(sizeof(file_layout_t));
    layout->filename = strdup(filename);
    layout->extents = (extent_info_t*)malloc(MAX_EXTENTS * sizeof(extent_info_t));

    // fiemap 구조체 할당
    fiemap = (struct fiemap*)malloc(sizeof(struct fiemap) + MAX_EXTENTS * sizeof(struct fiemap_extent));
    memset(fiemap, 0, sizeof(struct fiemap) + MAX_EXTENTS * sizeof(struct fiemap_extent));

    fiemap->fm_length = FIEMAP_MAX_OFFSET;
    fiemap->fm_flags = 0;
    fiemap->fm_extent_count = MAX_EXTENTS;

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        free(layout->extents);
        free(layout);
        free(fiemap);
        return NULL;
    }
	fsync(fd);

    if (ioctl(fd, FS_IOC_FIEMAP, fiemap) < 0) {
        perror("ioctl failed");
        close(fd);
        free(layout->extents);
        free(layout);
        free(fiemap);
        return NULL;
    }

    // extent 정보 복사
	  layout->start_offset = fiemap->fm_start;
    layout->extent_count = fiemap->fm_mapped_extents;
    layout->total_length = 0;

    for (int i = 0; i < layout->extent_count; i++) {
        layout->extents[i].logical_offset = fiemap->fm_extents[i].fe_logical;
        layout->extents[i].physical_offset = fiemap->fm_extents[i].fe_physical;
        layout->extents[i].length = fiemap->fm_extents[i].fe_length;
        //layout->extents[i].lba_start = fiemap->fm_extents[i].fe_physical / DEVICE_BLOCK_SIZE;
        //layout->extents[i].lba_count = fiemap->fm_extents[i].fe_length / DEVICE_BLOCK_SIZE;
        layout->extents[i].lba_start = fiemap->fm_extents[i].fe_physical;
        layout->extents[i].lba_count = fiemap->fm_extents[i].fe_length;
        layout->total_length += fiemap->fm_extents[i].fe_length;
    }

    close(fd);
    free(fiemap);
    return layout;
}

void free_file_layout(file_layout_t *layout) {
    if (layout) {
        free(layout->filename);
        free(layout->extents);
        free(layout);
    }
}
