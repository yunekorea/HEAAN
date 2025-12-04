#include "HEaaN_CWrapper.h"
#include "HEAAN.h"
#include <sstream>
#include <stdlib.h>

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

void* readCiphertextFromPath(char* path) {
  std::string pathString(path);
  Ciphertext* readCipher = SerializationUtils::readCiphertext(pathString);
  return readCipher;
}


int writeCiphertextToPath(void* cipher_ptr, char* path) {
  Ciphertext* cipherAdd = static_cast<Ciphertext*>(cipherAdd_ptr);
  std::string pathString(path);
  SerializationUtils::writeCiphertext(Ciphertext& cipher, string pathString);
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
