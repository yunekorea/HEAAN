#include "Profiling.h"

using namespace std;
using namespace NTL;
using namespace heaan;

int main() {
  /*
  * Basic Parameters are in src/Params.h
  * If you want to use another parameter, you need to change src/Params.h file and re-complie this library.
  */

  // Parameters //
  long logq = 300; ///< Ciphertext modulus (this value should be <= logQ in "scr/Params.h")
  long logp = 30; ///< Scaling Factor (larger logp will give you more accurate value)
  long logn = 10; ///< number of slot is 1024 (this value should be < logN in "src/Params.h")
  long n = 1 << logn;
  long slots = n;
  long numThread = 8;
	
  // Construct and Generate Public Keys //
  srand(time(NULL));
  //SetNumThreads(numThread);
  TimeUtils timeutils;
  Ring ring;
  SecretKey secretKey(ring);
  Scheme scheme(secretKey, ring);
  scheme.addLeftRotKeys(secretKey); ///< When you need left rotation for the vectorized message
  scheme.addRightRotKeys(secretKey); ///< When you need right rotation for the vectorized message
  
  // Make Random Array of Complex //
  //complex<double>* mvec1 = EvaluatorUtils::randomComplexArray(slots);
  //complex<double>* mvec2 = EvaluatorUtils::randomComplexArray(slots);

  //vector<complex<double>>* mvec0 = loadDouble("randint_1gb_0.txt");
  //vector<complex<double>>* mvec1 = loadDouble("randint_1gb_1.txt");
  //complex<double>* mvec0 = loadDouble(filename_0);
  //complex<double>* mvec1 = loadDouble(filename_1);
  
  //complex<double>* mvec0 = loadDouble("randint_1gb_0.txt");
  //complex<double>* mvec1 = loadDouble("randint_1gb_1.txt");
  int a = loadDouble(1);
  int b = loadDouble(2);

  /*
  // Encrypt Two Arry of Complex //
  Ciphertext cipher0;
  scheme.encrypt(cipher0, mvec0, n, logp, logq);
  Ciphertext cipher1;
  scheme.encrypt(cipher1, mvec1, n, logp, logq);
  */

  //saveCiphertext(cipher0, "randint_cipher_0.cip");
  //saveCiphertext(cipher1, "randint_cipher_1.cip");
  
  /*
  // Addition //
  Ciphertext cipherAdd;
  scheme.add(cipherAdd, cipher1, cipher2);
  
  // Multiplication And Rescale //
  Ciphertext cipherMult;
  scheme.mult(cipherMult, cipher1, cipher2);
  Ciphertext cipherMultAfterReScale;
  scheme.reScaleBy(cipherMultAfterReScale, cipherMult, logp);
  
  // Rotation //
  long idx = 1;
  Ciphertext cipherRot;
  scheme.leftRotateFast(cipherRot, cipher1, idx);
  
  // Decrypt //
  complex<double>* dvec1 = scheme.decrypt(secretKey, cipher1);
  complex<double>* dvec2 = scheme.decrypt(secretKey, cipher2);
  */


  return 0;

}
