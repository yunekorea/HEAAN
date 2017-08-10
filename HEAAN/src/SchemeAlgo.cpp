#include "SchemeAlgo.h"

#include <NTL/ZZ.h>
#include <NTL/BasicThreadPool.h>
#include <cmath>
#include <future>
#include <map>
#include <thread>

#include "Params.h"
#include "SchemeAux.h"
#include "EvaluatorUtils.h"

Cipher SchemeAlgo::powerOf2(Cipher& cipher, const long& precisionBits, const long& logDegree) {
	Cipher res = cipher;
	for (long i = 0; i < logDegree; ++i) {
		scheme.squareAndEqual(res);
		scheme.modSwitchAndEqual(res, precisionBits);
	}
	return res;
}

Cipher* SchemeAlgo::powerOf2Extended(Cipher& cipher, const long& precisionBits, const long& logDegree) {
	Cipher* res = new Cipher[logDegree + 1];
	res[0] = cipher;
	for (long i = 1; i < logDegree + 1; ++i) {
		res[i] = scheme.square(res[i-1]);
		scheme.modSwitchAndEqual(res[i], precisionBits);
	}
	return res;
}

//-----------------------------------------

Cipher SchemeAlgo::power(Cipher& cipher, const long& precisionBits, const long& degree) {
	long logDegree = log2(degree);
	long po2Degree = 1 << logDegree;

	Cipher res = powerOf2(cipher, precisionBits, logDegree);
	long remDegree = degree - po2Degree;
	if(remDegree > 0) {
		Cipher tmp = power(cipher, precisionBits, remDegree);
		long bitsDown = tmp.cbits - res.cbits;
		scheme.modEmbedAndEqual(tmp, bitsDown);
		scheme.multAndEqual(res, tmp);
		scheme.modSwitchAndEqual(res, precisionBits);
	}
	return res;
}

Cipher* SchemeAlgo::powerExtended(Cipher& cipher, const long& precisionBits, const long& degree) {
	Cipher* res = new Cipher[degree];
	long logDegree = log2(degree);
	Cipher* cpows = powerOf2Extended(cipher, precisionBits, logDegree);
	long idx = 0;
	for (long i = 0; i < logDegree; ++i) {
		long powi = (1 << i);
		res[idx++] = cpows[i];
		for (int j = 0; j < powi-1; ++j) {
			long bitsDown = res[j].cbits - cpows[i].cbits;
			res[idx] = scheme.modEmbed(res[j], bitsDown);
			scheme.multAndEqual(res[idx], cpows[i]);
			scheme.modSwitchAndEqual(res[idx++], precisionBits);
		}
	}
	res[idx++] = cpows[logDegree];
	long degree2 = (1 << logDegree);
	for (int i = 0; i < (degree - degree2); ++i) {
		long bitsDown = res[i].cbits - cpows[logDegree].cbits;
		res[idx] = scheme.modEmbed(res[i], bitsDown);
		scheme.multAndEqual(res[idx], cpows[logDegree]);
		scheme.modSwitchAndEqual(res[idx++], precisionBits);
	}
	return res;
}

//-----------------------------------------

Cipher SchemeAlgo::prodOfPo2(Cipher*& ciphers, const long& precisionBits, const long& logDegree) {
	Cipher* res = ciphers;
	for (long i = logDegree - 1; i >= 0; --i) {
		long powih = (1 << i);
		Cipher* tmp = new Cipher[powih];
		NTL_EXEC_RANGE(powih, first, last);
		for (long j = first; j < last; ++j) {
			tmp[j] = scheme.mult(res[2 * j], res[2 * j + 1]);
			scheme.modSwitchAndEqual(tmp[j], precisionBits);
		}
		NTL_EXEC_RANGE_END;
		res = tmp;
	}
	return res[0];
}

Cipher SchemeAlgo::prod(Cipher*& ciphers, const long& precisionBits, const long& degree) {
	long logDegree = log2(degree) + 1;
	long idx = 0;
	bool isinit = false;
	Cipher res;
	for (long i = 0; i < logDegree; ++i) {
		if(bit(degree, i)) {
			long powi = (1 << i);
			Cipher* tmp = new Cipher[powi];
			for (long j = 0; j < powi; ++j) {
				tmp[j] = ciphers[idx + j];
			}
			Cipher iprod = prodOfPo2(tmp, precisionBits, i);
			if(isinit) {
				long bitsDown = res.cbits - iprod.cbits;
				scheme.modEmbedAndEqual(res, bitsDown);
				scheme.multAndEqual(res, iprod);
				scheme.modSwitchAndEqual(res, precisionBits);
			} else {
				res = iprod;
				isinit = true;
			}
			idx += powi;
		}
	}
	return res;
}

Cipher SchemeAlgo::sum(Cipher*& ciphers, const long& size) {
	Cipher res = ciphers[0];
	for (long i = 1; i < size; ++i) {
		scheme.addAndEqual(res, ciphers[i]);
	}
	return res;
}

Cipher SchemeAlgo::distance(Cipher& cipher1, Cipher& cipher2, const long& precisionBits) {
	Cipher cres = scheme.sub(cipher1, cipher2);
	scheme.squareAndEqual(cres);
	scheme.modSwitchAndEqual(cres, precisionBits);
	partialSlotsSumAndEqual(cres, cres.slots);
	return cres;
}

Cipher* SchemeAlgo::multVec(Cipher*& ciphers1, Cipher*& ciphers2, const long& size) {
	Cipher* res = new Cipher[size];
	NTL_EXEC_RANGE(size, first, last);
	for (long i = first; i < last; ++i) {
		res[i] = scheme.mult(ciphers1[i], ciphers2[i]);
	}
	NTL_EXEC_RANGE_END;
	return res;
}

void SchemeAlgo::multAndEqualVec(Cipher*& ciphers1, Cipher*& ciphers2, const long& size) {
	NTL_EXEC_RANGE(size, first, last);
	for (long i = first; i < last; ++i) {
		scheme.multAndEqual(ciphers1[i], ciphers2[i]);
	}
	NTL_EXEC_RANGE_END;
}


Cipher* SchemeAlgo::multAndModSwitchVec(Cipher*& ciphers1, Cipher*& ciphers2, const long& precisionBits, const long& size) {
	Cipher* res = new Cipher[size];
	NTL_EXEC_RANGE(size, first, last);
	for (long i = first; i < last; ++i) {
		res[i] = scheme.mult(ciphers1[i], ciphers2[i]);
		scheme.modSwitchAndEqual(res[i], precisionBits);
	}
	NTL_EXEC_RANGE_END;
	return res;
}

void SchemeAlgo::multModSwitchAndEqualVec(Cipher*& ciphers1, Cipher*& ciphers2, const long& precisionBits, const long& size) {
	NTL_EXEC_RANGE(size, first, last);
	for (long i = first; i < last; ++i) {
		scheme.multAndEqual(ciphers1[i], ciphers2[i]);
		scheme.modSwitchAndEqual(ciphers1[i], precisionBits);
	}
	NTL_EXEC_RANGE_END;
}

CZZ* SchemeAlgo::decryptSingleArray(SecKey& secretKey, Cipher*& ciphers, long size) {
	CZZ* res = new CZZ[size];
	for (int i = 0; i < size; ++i) {
		Message msg = scheme.decryptMsg(secretKey, ciphers[i]);
		CZZ* gvals = scheme.decode(msg);
		res[i] = gvals[0];
	}
	return res;
}

Cipher* SchemeAlgo::encryptSingleArray(CZZ*& vals, long size) {
	Cipher* res = new Cipher[size];
	for (long i = 0; i < size; ++i) {
		res[i] = scheme.encryptSingle(vals[i]);
	}
	return res;
}

Cipher SchemeAlgo::innerProd(Cipher*& ciphers1, Cipher*& ciphers2, const long& precisionBits, const long& size) {
	Cipher cip = scheme.mult(ciphers1[size-1], ciphers2[size-1]);

	NTL_EXEC_RANGE(size-1, first, last);
	for (long i = first; i < last; ++i) {
		Cipher cprodi = scheme.mult(ciphers1[i], ciphers2[i]);
		scheme.addAndEqual(cip, cprodi);
	}
	NTL_EXEC_RANGE_END;

	scheme.modSwitchAndEqual(cip, precisionBits);
	return cip;
}
//-----------------------------------------

Cipher SchemeAlgo::inverse(Cipher& cipher, const long& precisionBits, const long& steps) {
	ZZ precision = power2_ZZ(precisionBits);
	Cipher cpow = cipher;
	Cipher tmp = scheme.addConst(cipher, precision);
	scheme.modEmbedAndEqual(tmp, precisionBits);
	Cipher res = tmp;

	for (long i = 1; i < steps; ++i) {
		scheme.squareAndEqual(cpow);
		scheme.modSwitchAndEqual(cpow, precisionBits);
		tmp = cpow;
		scheme.addConstAndEqual(tmp, precision);
		scheme.multAndEqual(tmp, res);
		scheme.modSwitchAndEqual(tmp, precisionBits);
		res = tmp;
	}
	return res;
}

Cipher* SchemeAlgo::inverseExtended(Cipher& cipher, const long& precisionBits, const long& steps) {
	ZZ precision = power2_ZZ(precisionBits);

	Cipher* res = new Cipher[steps];
	Cipher cpow = cipher;
	Cipher tmp = scheme.addConst(cipher, precision);
	scheme.modEmbedAndEqual(tmp, precisionBits);
	res[0] = tmp;

	for (long i = 1; i < steps; ++i) {
		scheme.squareAndEqual(cpow);
		scheme.modSwitchAndEqual(cpow, precisionBits);
		tmp = cpow;
		scheme.addConstAndEqual(tmp, precision);
		scheme.multAndEqual(tmp, res[i - 1]);
		scheme.modSwitchAndEqual(tmp, precisionBits);
		res[i] = tmp;
	}
	return res;
}

//-----------------------------------------

Cipher SchemeAlgo::function(Cipher& cipher, string& funcName, const long& precisionBits, const long& degree) {
	Cipher* cpows = powerExtended(cipher, precisionBits, degree);

	long dprecisionBits = 2 * precisionBits;

	double* coeffs = scheme.aux.taylorCoeffsMap.at(funcName);

	ZZ tmp = EvaluatorUtils::evaluateVal(coeffs[1], precisionBits);
	Cipher res = scheme.multByConst(cpows[0], tmp);

	tmp = EvaluatorUtils::evaluateVal(coeffs[0], dprecisionBits);
	scheme.addConstAndEqual(res, tmp);

	for (int i = 1; i < degree; ++i) {
		if(abs(coeffs[i + 1]) > 1e-27) {
			tmp = EvaluatorUtils::evaluateVal(coeffs[i + 1], precisionBits);
			Cipher aixi = scheme.multByConst(cpows[i], tmp);
			long bitsDown = res.cbits - aixi.cbits;
			scheme.modEmbedAndEqual(res, bitsDown);
			scheme.addAndEqual(res, aixi);
		}
	}
	scheme.modSwitchAndEqual(res, precisionBits);
	return res;
}

Cipher SchemeAlgo::functionLazy(Cipher& cipher, string& funcName, const long& precisionBits, const long& degree) {
	Cipher* cpows = powerExtended(cipher, precisionBits, degree);

	long dprecisionBits = 2 * precisionBits;

	double* coeffs = scheme.aux.taylorCoeffsMap.at(funcName);

	ZZ tmp = EvaluatorUtils::evaluateVal(coeffs[1], precisionBits);
	Cipher res = scheme.multByConst(cpows[0], tmp);

	tmp = EvaluatorUtils::evaluateVal(coeffs[0], dprecisionBits);
	scheme.addConstAndEqual(res, tmp);

	for (int i = 1; i < degree; ++i) {
		if(abs(coeffs[i + 1]) > 1e-27) {
			tmp = EvaluatorUtils::evaluateVal(coeffs[i + 1], precisionBits);
			Cipher aixi = scheme.multByConst(cpows[i], tmp);
			long bitsDown = res.cbits - aixi.cbits;
			scheme.modEmbedAndEqual(res, bitsDown);
			scheme.addAndEqual(res, aixi);
		}
	}
	return res;
}

Cipher* SchemeAlgo::functionExtended(Cipher& cipher, string& funcName, const long& precisionBits, const long& degree) {
	Cipher* cpows = powerExtended(cipher, precisionBits, degree);

	long dprecisionBits = 2 * precisionBits;
	double* coeffs = scheme.aux.taylorCoeffsMap.at(funcName);

	ZZ tmp = EvaluatorUtils::evaluateVal(coeffs[1], precisionBits);
	Cipher aixi = scheme.multByConst(cpows[0], tmp);

	tmp = EvaluatorUtils::evaluateVal(coeffs[0], dprecisionBits);
	scheme.addConstAndEqual(aixi, tmp);

	Cipher* res = new Cipher[degree];
	res[0] = aixi;
	for (long i = 1; i < degree; ++i) {
		if(abs(coeffs[i + 1]) > 1e-27) {
			tmp = EvaluatorUtils::evaluateVal(coeffs[i + 1], precisionBits);
			aixi = scheme.multByConst(cpows[i], tmp);
			long bitsDown = res[i - 1].cbits - aixi.cbits;
			Cipher ctmp = scheme.modEmbed(res[i - 1], bitsDown);
			scheme.addAndEqual(aixi, ctmp);
			res[i] = aixi;
		} else {
			res[i] = res[i - 1];
		}
	}
	NTL_EXEC_RANGE(degree, first, last);
	for (long i = first; i < last; ++i) {
		scheme.modSwitchAndEqual(res[i], precisionBits);
	}
	NTL_EXEC_RANGE_END;
	return res;
}

//-----------------------------------------

Cipher* SchemeAlgo::fftRaw(Cipher*& ciphers, const long& size, const bool& isForward) {

	if(size == 1) {
		return ciphers;
	}

	long sizeh = size >> 1;

	Cipher* sub1 = new Cipher[sizeh];
	Cipher* sub2 = new Cipher[sizeh];

	for (long i = 0; i < sizeh; ++i) {
		sub1[i] = ciphers[2 * i];
		sub2[i] = ciphers[2 * i + 1];
	}

	future<Cipher*> f1 = async(&SchemeAlgo::fftRaw, this, ref(sub1), ref(sizeh), ref(isForward));
	future<Cipher*> f2 = async(&SchemeAlgo::fftRaw, this, ref(sub2), ref(sizeh), ref(isForward));

	Cipher* y1 = f1.get();
	Cipher* y2 = f2.get();

	long shift = isForward ? ((scheme.params.N / size) << 1) : ((scheme.params.N - scheme.params.N / size) << 1);

	Cipher* res = new Cipher[size];

	thread* thpool = new thread[sizeh];
	for (long i = 0; i < sizeh; ++i) {
		thpool[i] = thread(&SchemeAlgo::butOperation, this, ref(res[i]), ref(res[i + sizeh]), ref(y1[i]), ref(y2[i]), shift * i);
	}
	for (long i = 0; i < sizeh; ++i) {
		thpool[i].join();
	}

	return res;
}

void SchemeAlgo::butOperation(Cipher& res1, Cipher& res2, Cipher& y1, Cipher& y2, long shift) {
	scheme.multByMonomialAndEqual(y2, shift);
	res1 = y1;
	scheme.addAndEqual(res1, y2);
	res2 = y1;
	scheme.subAndEqual(res2, y2);
}

Cipher* SchemeAlgo::fft(Cipher*& ciphers, const long& size) {
	return fftRaw(ciphers, size, true);
}

Cipher* SchemeAlgo::fftInv(Cipher*& ciphers, const long& size) {
	Cipher* fftInv = fftRaw(ciphers, size, false);
	long logsize = log2(size);

	NTL_EXEC_RANGE(size, first, last);
	for (long i = first; i < last; ++i) {
		scheme.modSwitchAndEqual(fftInv[i], logsize);
	}
	NTL_EXEC_RANGE_END;

	return fftInv;
}

Cipher* SchemeAlgo::fftInvLazy(Cipher*& ciphers, const long& size) {
	return fftRaw(ciphers, size, false);
}

Cipher SchemeAlgo::partialSlotsSum(Cipher& cipher, const long& slots) {
	long logslots = log2(slots);
	Cipher res = cipher;
	for (long i = 0; i < logslots; ++i) {
		Cipher rot = scheme.leftRotateByPo2(cipher, i);
		scheme.addAndEqual(res, rot);
	}
	return res;
}

void SchemeAlgo::partialSlotsSumAndEqual(Cipher& cipher, const long& slots) {
	long logslots = log2(slots);
	for (long i = 0; i < logslots; ++i) {
		Cipher rot = scheme.leftRotateByPo2(cipher, i);
		scheme.addAndEqual(cipher, rot);
	}
}
