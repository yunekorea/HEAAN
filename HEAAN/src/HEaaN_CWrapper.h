#include <stddef.h>

#ifndef HEAAN_CWRAPPER_H
#define HEAAN_CWRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

void* create_Ciphertext(void);
void* create_Ring(void);
void* create_SecretKey(void* ring_ptr);
void* create_Scheme(void* secretKey_ptr, void* ring_ptr);
void addLeftRotKeys(void* scheme_ptr, void *secretKey_ptr);
void addRightRotKeys(void* scheme_ptr, void *secretKey_ptr);
int ciphertextAdd(void* scheme_ptr, void* cipherAdd_ptr, void* cipher1_ptr, void* cipher2_ptr);
void* readCiphertextFromMem(void* buffer, size_t len);

#ifdef __cplusplus
}
#endif

#endif // HEAAN_CWRAPPER_H
