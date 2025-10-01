#include "HEaaN_CWrapper.h"
#include "HEAAN.h"
#include <sstream>

using namespace std;
using namespace NTL;
using namespace heaan;

extern "C" {

void* create_Ciphertext(void) {
  return new Ciphertext();
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

int ciphertextAdd(void* scheme_ptr, void* cipherAdd_ptr, void* cipher1_ptr, void* cipher2_ptr) {
  Scheme* scheme = static_cast<Scheme*>(scheme_ptr);
  Ciphertext* cipherAdd = static_cast<Ciphertext*>(cipherAdd_ptr); 
  Ciphertext* cipher1 = static_cast<Ciphertext*>(cipher1_ptr); 
  Ciphertext* cipher2 = static_cast<Ciphertext*>(cipher2_ptr); 
  scheme->add(*cipherAdd, *cipher1, *cipher2);
  return 0;
}

void* readCiphertextFromMem(void* buffer, size_t len) {
  long n, logp, logq; 
  
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

  delete[] bytes;

  return cipher;
}

}
