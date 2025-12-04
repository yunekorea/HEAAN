#include "ProfilingNDP.h"

using namespace std;
using namespace NTL;
using namespace heaan;


std::complex<double>* loadDouble(const std::string FileName);
void* data_buffer_alloc(size_t len);

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

  
  //int iter = 214600;
  int iter = 10;
  std::string oper = string(argv[1]);
  cout << "iterator setup done" << endl;
  cout << "Selected Operation : " << oper << endl;
  // Construct and Generate Public Keys //

  
  // Encrypt to ciphertexts //

  // Load/add ciphertexts //
  if(oper == "cipadd"){
    std::string temp;
    std::string input_0_path;
    std::string input_0_filename;
    std::string input_1_path;
    std::string input_1_filename;
    std::string target_path;
    std::string target_filename;
    //complex<double>* dvecadd[iter];
    for(int i = 0; i < iter; i++) { 
      cout << "Progress : " << i << "/" << iter-1 << "\n";
      
      input_0_path = WorkingDir + "/1GB_ciphertexts/";
      //strcpy(input_0_path, temp.c_str());
      input_0_filename = SFileName0 + std::to_string(i) + ".cip";
      //strcpy(input_0_filename, temp.c_str());
      cout << input_0_path << input_0_filename << endl;
      
      input_1_path = WorkingDir + "/1GB_ciphertexts/";
      //strcpy(input_1_path, temp.c_str());
      input_1_filename = SFileName1 + std::to_string(i) + ".cip";
      //strcpy(input_1_filename, temp.c_str());
      cout << input_1_path << input_1_filename << endl;
      
      target_path = WorkingDir + "/1GB_ciphertexts/";
      //strcpy(target_path, temp.c_str());
      target_filename = SFileNameAdd + std::to_string(i) + ".cip";
      //strcpy(target_filename, temp.c_str());
      cout << target_path << target_filename << endl;

      size_t full_string_len = input_0_path.length() + input_0_filename.length() +
									input_1_path.length() + input_1_filename.length() +
									target_path.length() + target_filename.length() + 6;
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

      void* data_buffer = data_buffer_alloc(cipadd_cmd.data_len);
      memset(data_buffer, 0, cipadd_cmd.data_len);
      std::snprintf((char*)data_buffer, full_string_len,
              "%s|%s|%s|%s|%s|%s",
              input_0_path.c_str(), input_0_filename.c_str(),
              input_1_path.c_str(), input_1_filename.c_str(),
              target_path.c_str(), target_filename.c_str());
      
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
