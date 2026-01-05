#include "HEaaN_CWrapper.h"
#include "HEAAN.h"
#include <sstream>
#include <stdlib.h>

#include <fcntl.h>  // For open, O_DIRECT
#include <unistd.h> // For read, close
#include <stdlib.h> // For posix_align

using namespace std;
using namespace NTL;
using namespace heaan;

extern "C" {

//Global pointer for HEaaN to work with SPDK
static heaan_ndp_context* heaan_context = NULL;

int heaan_Initialize(
                int logq, int logp, int logn,
                int numThread, int iter) {
  if(heaan_context == NULL)
    heaan_context = (heaan_ndp_context*)malloc(sizeof(heaan_context));
  heaan_context->logq = (long)logq;
  heaan_context->logp = (long)logp;
  heaan_context->logn = (long)logn;
  heaan_context->n = 1 << (long)logn;
  heaan_context->slots = heaan_context->n;
  heaan_context->numThread = (long)numThread;
  heaan_context->iter = iter;
  heaan_context->ring = create_Ring();
  heaan_context->secretKey = create_SecretKey(heaan_context->ring); 
  heaan_context->scheme = create_Scheme(heaan_context->secretKey, heaan_context->ring);
  addLeftRotKeys(heaan_context->scheme, heaan_context->secretKey);
  addRightRotKeys(heaan_context->scheme, heaan_context->secretKey);

  return 0;
}

heaan_ndp_context* heaan_Get_Context(void) {
  return heaan_context;
}

void* create_Ciphertext(void) {
  return new Ciphertext();
}

void free_Ciphertext(void* cipher_ptr) {
  Ciphertext* ciphertext = static_cast<Ciphertext*>(cipher_ptr); 
  delete ciphertext;
}

void* create_Ring(void) {
  return new Ring();
}

void* create_SecretKey(void* ring_ptr) {
  Ring* ring = static_cast<Ring*>(ring_ptr);

  return new SecretKey(*ring);
}

void* create_Scheme(void* secretKey_ptr, void* ring_ptr) {
  Ring* ring = static_cast<Ring*>(ring_ptr);
  SecretKey* secretKey = static_cast<SecretKey*>(secretKey_ptr);
  return new Scheme(*secretKey, *ring);
}

void addLeftRotKeys(void* scheme_ptr, void* secretKey_ptr) {
  Scheme* scheme = static_cast<Scheme*>(scheme_ptr);
  SecretKey* secretKey = static_cast<SecretKey*>(secretKey_ptr);
  scheme->addLeftRotKeys(*secretKey);
}

void addRightRotKeys(void* scheme_ptr, void *secretKey_ptr) {
  Scheme* scheme = static_cast<Scheme*>(scheme_ptr);
  SecretKey* secretKey = static_cast<SecretKey*>(secretKey_ptr);
  scheme->addRightRotKeys(*secretKey);
}

long getCiphertextN(void* cipher_ptr) {
  Ciphertext* cipher = static_cast<Ciphertext*>(cipher_ptr);
  return cipher->n;
}

long getCiphertextLogp(void* cipher_ptr) {
  Ciphertext* cipher = static_cast<Ciphertext*>(cipher_ptr);
  return cipher->logp;
}

long getCiphertextLogq(void* cipher_ptr) {
  Ciphertext* cipher = static_cast<Ciphertext*>(cipher_ptr);
  return cipher->logq;
}

int ciphertextAdd(void* scheme_ptr, void* cipherAdd_ptr, void* cipher1_ptr, void* cipher2_ptr) {
  Scheme* scheme = static_cast<Scheme*>(scheme_ptr);
  Ciphertext* cipherAdd = static_cast<Ciphertext*>(cipherAdd_ptr); 
  Ciphertext* cipher1 = static_cast<Ciphertext*>(cipher1_ptr); 
  Ciphertext* cipher2 = static_cast<Ciphertext*>(cipher2_ptr); 
  scheme->add(*cipherAdd, *cipher1, *cipher2);
  return 0;
}

void* readCiphertextFromPath(char* path) {
  std::string pathString(path);
	long n, logp, logq;
  const size_t ALIGN = 4096;
  
  cout << "ReadCiphertextFromPath\npathString: " << pathString << endl;
	cout << "readCihpertextFromPath: BEGIN" << endl;
	
  cout << "readCihpertextFromPath: Sleep 60 seconds(DEBUG)" << endl;
  sleep(60);
  return nullptr;

  int fd = open(path, O_RDONLY | O_DIRECT);
  if(fd < 0) {
    cout << "readCiphertextFromPath: open FAILED" << endl;
    return nullptr;
  }
	cout << "readCihpertextFromPath: open" << endl;
	
  void* parameter_buf = nullptr;
  if(posix_memalign(&parameter_buf, ALIGN, ALIGN) != 0) {
    cout << "readCiphertextFromPath: parameter buffer init FAILED" << endl;
    close(fd);
    return nullptr;
  }
  if(read(fd, parameter_buf, ALIGN) < (ssize_t)(3*sizeof(long))) {
    cout << "readCiphertextFromPath: parameter buffer read FAILED" << endl;
    close(fd);
    return nullptr;
  }
  long* parameter_ptr = reinterpret_cast<long*>(parameter_buf);
  n = parameter_ptr[0];
  logp = parameter_ptr[1];
  logq = parameter_ptr[2];
	cout << "readCihpertextFromPath: Parameter read DONE" << endl;
  
	long np = ceil(((double)logq + 1)/8);
	Ciphertext* cipher = new Ciphertext(logp, logq, n);
	cout << "readCihpertextFromPath: Ciphertext class created" << endl;

  size_t ciphertext_read_size = ((np + ALIGN - 1) / ALIGN) * ALIGN; 
  void* ciphertext_buf = nullptr;
  if(posix_memalign(&ciphertext_buf, ALIGN, ciphertext_read_size) != 0) {
    cout << "readCiphertextFromPath: ciphertext buffer init FAILED" << endl;
    close(fd);
    return nullptr;
  }

	for (long i = 0; i < N; ++i) {
		if (read(fd, ciphertext_buf, ciphertext_read_size) < (ssize_t)np) {
      cout << "readCiphertextFromPath: Failed to read ax[" << i << "]" << endl;
      break;
    }
    ZZFromBytes(cipher->ax[i], reinterpret_cast<unsigned char*>(ciphertext_buf), np);
	}
	for (long i = 0; i < N; ++i) {
		if (read(fd, ciphertext_buf, ciphertext_read_size) < (ssize_t)np) {
      cout << "readCiphertextFromPath: Failed to read bx[" << i << "]" << endl;
      break;
    }
    ZZFromBytes(cipher->bx[i], reinterpret_cast<unsigned char*>(ciphertext_buf), np);
	}
  close(fd);
  free(parameter_buf);
  free(ciphertext_buf);
	cout << "readCihpertextFromPath: closed" << endl;
  cout << "ReadCiphertextFromPath\nDONE" << pathString << endl;
	return cipher;
}


int writeCiphertextToPath(void* cipher_ptr, char* path) {
  Ciphertext* cipher = static_cast<Ciphertext*>(cipher_ptr);
  std::string pathString(path);
  cout << "WriteCiphertextToPath\npathString: " << pathString << endl;
  SerializationUtils::writeCiphertext(*cipher, pathString);
  cout << "WriteCiphertextToPath\nDONE" << pathString << endl;
  return 0;
}

void* readCiphertextFromMem(void* buffer, size_t len, uint64_t offset) {
  long n, logp, logq; 
  
  uint8_t* dataPtr = static_cast<uint8_t*>(buffer) + offset; 
  
  memcpy(&n, dataPtr, sizeof(long));    dataPtr += sizeof(long);
  memcpy(&logp, dataPtr, sizeof(long)); dataPtr += sizeof(long);
  memcpy(&logq, dataPtr, sizeof(long)); dataPtr += sizeof(long); 
  
  long np = ceil(((double)logq + 1) / 8);
  unsigned char* bytes = new unsigned char[np];
  Ciphertext* cipher = new Ciphertext(logp, logq, n);
 
  for (long i = 0; i < N; ++i) {
        memcpy(bytes, dataPtr, np);
        ZZFromBytes(cipher->ax[i], bytes, np);
        dataPtr += np;
    }

    for (long i = 0; i < N; ++i) {
        memcpy(bytes, dataPtr, np);
        ZZFromBytes(cipher->bx[i], bytes, np);
        dataPtr += np;
    }
  
  /*
  std::string dataBuffer(static_cast<char*>(buffer), len);
  std::stringstream dataStream(dataBuffer);

  dataStream.read(reinterpret_cast<char*>(&n), sizeof(long));
  dataStream.read(reinterpret_cast<char*>(&logp), sizeof(long));
  dataStream.read(reinterpret_cast<char*>(&logq), sizeof(long));

  long np = ceil(((double)logq + 1)/8);
  unsigned char* bytes = new unsigned char[np];
  Ciphertext* cipher = new Ciphertext(logp, logq, n);
  for(long i = 0; i < N; ++i) {
    dataStream.read(reinterpret_cast<char*>(bytes), np);
    ZZFromBytes(cipher->ax[i], bytes, np);
  }
  for(long i = 0; i < N; ++i) {
    dataStream.read(reinterpret_cast<char*>(bytes), np);
    ZZFromBytes(cipher->bx[i], bytes, np);
  }
  */

  delete[] bytes;

  return cipher;
}

}
