#include <stddef.h>
#include <stdint.h>

#ifndef HEAAN_CWRAPPER_H
#define HEAAN_CWRAPPER_H

// Define the macro to ensure C linkage for all wrapper functions
#ifdef __cplusplus
#define HEAAN_C_API extern "C"
#else
#define HEAAN_C_API
#endif

// Correct, portable C typedef syntax
typedef struct _heaan_ndp_context {
    long logq;
    long logp;
    long logn;
    long n;
    long slots;
    long numThread;
    int iter;
    void* ring;
    void* secretKey;
    void* scheme;
} heaan_ndp_context; // CRITICAL: Typedef name moved here

HEAAN_C_API int heaan_Initialize(
    int logq, int logp, int logn,
    int numThread, int iter);

HEAAN_C_API heaan_ndp_context* heaan_Get_Context(void);

HEAAN_C_API void* create_Ciphertext(void);
HEAAN_C_API void free_Ciphertext(void* cipher_ptr);
HEAAN_C_API void* create_Ring(void);
HEAAN_C_API void* create_SecretKey(void* ring_ptr);
HEAAN_C_API void* create_Scheme(void* secretKey_ptr, void* ring_ptr);
HEAAN_C_API void addLeftRotKeys(void* scheme_ptr, void *secretKey_ptr);
HEAAN_C_API void addRightRotKeys(void* scheme_ptr, void *secretKey_ptr);
HEAAN_C_API long getCiphertextN(void* cipher_ptr);
HEAAN_C_API long getCiphertextLogp(void* cipher_ptr);
HEAAN_C_API long getCiphertextLogq(void* cipher_ptr);
HEAAN_C_API int ciphertextAdd(void* scheme_ptr, void* cipherAdd_ptr, void* cipher1_ptr, void* cipher2_ptr);
HEAAN_C_API void* readCiphertextFromPath(char* path);
HEAAN_C_API int writeCiphertextToPath(void* cipher_ptr, char* path);
HEAAN_C_API void* readCiphertextFromMem(void* buffer, size_t len, uint64_t offset);


#endif // HEAAN_CWRAPPER_H
