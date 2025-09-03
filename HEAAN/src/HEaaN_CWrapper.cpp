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

void* create_SecretKey(void* ring) {
  return new SecretKey((Ring*)ring);
}

void* create_Scheme(void* secretKey, void* ring) {
  return new Scheme((secretKey*) secretKey, (Ring*) ring);
}

void addLeftRotKeys(void* scheme, void *secretKey) {
  ((Scheme*)scheme).addLeftRotKeys((SecretKey*)secretKey);
}

void addRightRotKeys(void* scheme, void *secretKey) {
  ((Scheme*)scheme).addRightRotKeys((SecretKey*)secretKey);
}

}
