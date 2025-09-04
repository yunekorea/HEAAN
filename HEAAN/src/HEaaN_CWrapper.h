#include "BootContext.h"
#include "Ring.h"
#include "RingMultiplier.h"
#include "Ciphertext.h"
#include "EvaluatorUtils.h"
#include "Scheme.h"
#include "SchemeAlgo.h"
#include "SecretKey.h"
#include "StringUtils.h"
#include "TimeUtils.h"
#include "SerializationUtils.h"
#include "TestScheme.h"
#include "Params.h"

#ifndef HEAAN_CWRAPPER_H
#define HEAAN_CWRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

void* create_Ciphertext(void);
void* create_Ring(void);
void* create_SecretKey(void* ring);
void* create_Scheme(void* secretKey, void* ring);
void addLeftRotKeys(void* scheme, void *secretKey);
void addRightRotKeys(void* scheme, void *secretKey);

#ifdef __cplusplus
}
#endif

#endif // HEAAN_CWRAPPER_H
