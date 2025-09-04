#include "HEaaN_CWrapper.h"

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

}
