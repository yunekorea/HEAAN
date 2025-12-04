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
  int nvme_fd = nvme_open("/dev/nvme1n1");
  if(nvme_fd == -1) {
    cout << "Opening the file descriptor has failed." << endl;
    return -1;
  }

  // Parameters //
  long logq = 300; ///< Ciphertext modulus (this value should be <= logQ in "scr/Params.h")
  long logp = 30; ///< Scaling Factor (larger logp will give you more accurate value)
  long logn = 10; ///< number of slot is 1024 (this value should be < logN in "src/Params.h")
  long n = 1 << logn;
  long slots = n;
  
  //int iter = 214600;
  int iter = 1024;
  std::string oper = string(argv[1]);
  cout << "iterator setup done" << endl;
  cout << "Selected Operation : " << oper << endl;
  // Construct and Generate Public Keys //
  srand(time(NULL));
  long numThread = 8;
  SetNumThreads(numThread);
  TimeUtils timeutils;
  Ring ring;
  SecretKey* secretKeyPointer = NULL;
  if(oper == "enc") {
    secretKeyPointer = new SecretKey(ring);
    SerializationUtils::writeSecretKey(*secretKeyPointer, WorkingDir + "/HEaaN_SecretKey/secretKey.sk");
  }
  else {
    secretKeyPointer = SerializationUtils::readSecretKey(ring, WorkingDir + "/HEaaN_SecretKey/secretKey.sk");
  }
  SecretKey secretKey = *secretKeyPointer; 
  free(secretKeyPointer);
  Scheme scheme(secretKey, ring);
  scheme.addLeftRotKeys(secretKey); ///< When you need left rotation for the vectorized message
  scheme.addRightRotKeys(secretKey); ///< When you need right rotation for the vectorized message

  // Make Random Array of Complex //
  //complex<double>* mvec0 = EvaluatorUtils::randomComplexArray(slots);
  //complex<double>* mvec1 = EvaluatorUtils::randomComplexArray(slots);
  
  // Encrypt to ciphertexts //
  if(oper == "enc") {
  Ciphertext cipher0[iter];
  Ciphertext cipher1[iter];
  complex<double>* mvec0[iter];
  complex<double>* mvec1[iter];
  for(int i = 0; i < iter; i++) {
    cout << "Progress : " << i << "/" << iter-1 << "\r"; 
    mvec0[i] = loadDouble("randint_1024_0.txt");
    mvec1[i] = loadDouble("randint_1024_1.txt");

    scheme.encrypt(cipher0[i], mvec0[i], n, logp, logq);
    scheme.encrypt(cipher1[i], mvec1[i], n, logp, logq);

    SerializationUtils::writeCiphertext(cipher0[i], WorkingDir + "/1GB_ciphertexts/" + SFileName0 + std::to_string(i) + ".cip");
    SerializationUtils::writeCiphertext(cipher1[i], WorkingDir + "/1GB_ciphertexts/" + SFileName1 + std::to_string(i) + ".cip");
    }
    cout << endl;
  }

  // Load/add ciphertexts //
  if(oper == "cipadd"){
    std::string temp;
    char* input_0_path = NULL;
    char* input_0_filename = NULL;
    char* input_1_path = NULL;
    char* input_1_filename = NULL;
    char* target_path = NULL;
    char* target_filename = NULL;
    //complex<double>* dvecadd[iter];
    for(int i = 0; i < iter; i++) { 
      cout << "Progress : " << i << "/" << iter-1 << "\r";
      
      temp = WorkingDir + "/1GB_ciphertexts/";
      strcpy(input_0_path, temp.c_str());
      temp = SFileName0 + std::to_string(i) + ".cip";
      strcpy(input_0_filename, temp.c_str());
      
      temp = WorkingDir + "/1GB_ciphertexts/";
      strcpy(input_1_path, temp.c_str());
      temp = SFileName1 + std::to_string(i) + ".cip";
      strcpy(input_1_filename, temp.c_str());
      
      temp = WorkingDir + "/1GB_ciphertexts/";
      strcpy(target_path, temp.c_str());
      temp = SFileNameAdd + std::to_string(i) + ".cip";
      strcpy(target_filename, temp.c_str());

      size_t full_string_len = strlen(input_0_path) + strlen(input_0_filename) +
									strlen(input_1_path) + strlen(input_1_filename) +
									strlen(target_path) + strlen(target_filename) + 5;
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
      std::snprintf((char*)data_buffer, full_string_len,
              "%s|%s|%s|%s|%s|%s",
              input_0_path, input_0_filename,
              input_1_path, input_1_filename,
              target_path, target_filename);
      
      cipadd_cmd.data = data_buffer;

      int result = nvme_admin_passthru64(nvme_fd, cipadd_cmd.opcode, cipadd_cmd.flags,
                      cipadd_cmd.rsvd, cipadd_cmd.nsid, cipadd_cmd.cdw2, cipadd_cmd.cdw3,
                      cipadd_cmd.cdw10, cipadd_cmd.cdw11, cipadd_cmd.cdw12, cipadd_cmd.cdw13,
                      cipadd_cmd.cdw14, cipadd_cmd.cdw15, cipadd_cmd.data_len, cipadd_cmd.data,
                      cipadd_cmd.metadata_len, cipadd_cmd.metadata, cipadd_cmd.timeout_ms, 
                      &cipadd_cmd.result);
    
    }
    cout << endl;
  }
  
  // multiply ciphertexts //
  if(oper == "cipmult") {
  Ciphertext* cipher4[iter];
  Ciphertext* cipher5[iter];
  Ciphertext cipherMult[iter];
  //complex<double>* dvecmult[iter];
  for(int i = 0; i < iter; i++) { 
    cout << "Progress : " << i << "/" << iter-1 << "\r"; 
    cipher4[i] = SerializationUtils::readCiphertext(WorkingDir + "/1GB_ciphertexts/" + SFileName0 + std::to_string(i) + ".cip");
    cipher5[i] = SerializationUtils::readCiphertext(WorkingDir + "/1GB_ciphertexts/" + SFileName1 + std::to_string(i) + ".cip");

    scheme.mult(cipherMult[i], *cipher4[i], *cipher5[i]);
    SerializationUtils::writeCiphertext(cipherMult[i], WorkingDir + "/1GB_ciphertexts/" + SFileName0 + std::to_string(i) + ".cip");

    //dvecmult[i] = scheme.decrypt(secretKey, cipherMult[i]);
  }
  cout << endl;
  }

  // Add decrypted //
  if(oper == "decadd") {
  Ciphertext* cipher6[iter];
  Ciphertext* cipher7[iter];
  complex<double>* dvec0[iter];
  complex<double>* dvec1[iter];
  complex<double>* dvecadd[iter];
  for(int i = 0; i < iter; i++) { 
    cout << "Progress : " << i << "/" << iter-1 << "\r"; 
    cipher6[i] = SerializationUtils::readCiphertext(WorkingDir + "/1GB_ciphertexts/" + SFileName0 + std::to_string(i) + ".cip");
    cipher7[i] = SerializationUtils::readCiphertext(WorkingDir + "/1GB_ciphertexts/" + SFileName1 + std::to_string(i) + ".cip");

    dvec0[i] = scheme.decrypt(secretKey, *cipher6[i]);
    dvec1[i] = scheme.decrypt(secretKey, *cipher7[i]);

    complex<double>* dvectemp = new complex<double>[1024];
    for(int j = 0; j < 1024; j++) {
      dvectemp[j] = dvec0[i][j] + dvec1[i][j];
      dvecadd[i] = dvectemp;
    }
  }
    cout << endl;
    }
  
  // Multiply decrypted //
  if(oper == "decmult") {
  Ciphertext* cipher8[iter];
  Ciphertext* cipher9[iter];
  complex<double>* dvec2[iter];
  complex<double>* dvec3[iter];
  complex<double>* dvecmult[iter];
  for(int i = 0; i < iter; i++) { 
    cout << "Progress : " << i << "/" << iter-1 << "\r"; 
    cipher8[i] = SerializationUtils::readCiphertext(WorkingDir + "/1GB_ciphertexts/" + SFileName0 + std::to_string(i) + ".cip");
    cipher9[i] = SerializationUtils::readCiphertext(WorkingDir + "/1GB_ciphertexts/" + SFileName1 + std::to_string(i) + ".cip");

    dvec2[i] = scheme.decrypt(secretKey, *cipher8[i]);
    dvec3[i] = scheme.decrypt(secretKey, *cipher9[i]);

    complex<double>* dvectemp = new complex<double>[1024];
    for(int j = 0; j < 1024; j++) {
      dvectemp[j] = dvec2[i][j] * dvec3[i][j];
      dvecmult[i] = dvectemp;
    }
    }
    cout << endl;
  }

  /*
  // Multiplication And Rescale //
  Ciphertext cipherMult;
  scheme.mult(cipherMult, *cipher2, *cipher3);
  Ciphertext cipherMultAfterReScale;
  scheme.reScaleBy(cipherMultAfterReScale, cipherMult, logp);
  
  // Rotation //
  long idx = 1;
  Ciphertext cipherRot;
  scheme.leftRotateFast(cipherRot, cipher1, idx);
  */
  // Decrypt //
  /*
  cout << "Decrypt" << endl;
  complex<double>* dvec0 = scheme.decrypt(secretKey, *cipher2);
  complex<double>* dvec1 = scheme.decrypt(secretKey, *cipher3);
  cout << "DONE" << endl;

  complex<double> dvecadd[heaan::N];
  for(int i = 0; i < heaan::N; i++) {
    dvecadd[i] = dvec0[i] + dvec1[i];
  }
  */

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
