#include "Profiling.h"

using namespace std;
using namespace NTL;
using namespace heaan;


std::complex<double>* loadDouble(const std::string FileName);

int main(int argc, char **argv) {
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
  
  int iter = 1 << 10;
  cout << "iterator setup done" << endl;
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
  //complex<double>* mvec0 = EvaluatorUtils::randomComplexArray(slots);
  //complex<double>* mvec1 = EvaluatorUtils::randomComplexArray(slots);

  
  std::string SFileName0 = "randint_cipher_1024_0_";
  std::string SFileName1 = "randint_cipher_1024_1_";
  
  // Encrypt to ciphertexts //
  if(string(argv[1]) == "enc") {
  Ciphertext cipher0[iter];
  Ciphertext cipher1[iter];
  complex<double>* mvec0[iter];
  complex<double>* mvec1[iter];
  cout << "ciphersetup DONE" << endl;
  for(int i = 0; i < iter; i++) {
    cout << "loading double : " << i << " : "; 
    mvec0[i] = loadDouble("randint_1024_0.txt");
    mvec1[i] = loadDouble("randint_1024_1.txt");
    cout << "DONE" << endl;

    cout << "Encrypting : " << i << " : ";
    scheme.encrypt(cipher0[i], mvec0[i], n, logp, logq);
    scheme.encrypt(cipher1[i], mvec1[i], n, logp, logq);
    cout << "DONE" << endl;

    cout << "saving ciphertext : " << i << " : ";
    SerializationUtils::writeCiphertext(cipher0[i], "./MIL_ciphertexts/" + SFileName0 + std::to_string(i) + ".cip");
    SerializationUtils::writeCiphertext(cipher1[i], "./MIL_ciphertexts/" + SFileName1 + std::to_string(i) + ".cip");
    cout << "DONE" << endl;
  }
  }

  // Load/add ciphertexts //
  if(string(argv[1]) == "cipadd"){
  Ciphertext* cipher2[iter];
  Ciphertext* cipher3[iter];
  Ciphertext cipherAdd[iter];
  complex<double>* dvecadd[iter];
  for(int i = 0; i < iter; i++) { 
    cout << "Reading ciphertext : " << i << " : ";
    cipher2[i] = SerializationUtils::readCiphertext("./MIL_ciphertexts/" + SFileName0 + std::to_string(i) + ".cip");
    cipher3[i] = SerializationUtils::readCiphertext("./MIL_ciphertexts/" + SFileName1 + std::to_string(i) + ".cip");
    cout << "DONE" << endl;

    cout << "Ciphertext add : " << i << " : ";
    scheme.add(cipherAdd[i], *cipher2[i], *cipher3[i]);
    cout << "DONE" << endl;

    cout << "Decrypt : " << i << " : ";
    dvecadd[i] = scheme.decrypt(secretKey, cipherAdd[i]);
    cout << "DONE" << endl;

  }
  }
  
  // multiply ciphertexts //
  if(string(argv[1]) == "cipmult") {
  Ciphertext* cipher4[iter];
  Ciphertext* cipher5[iter];
  Ciphertext cipherMult[iter];
  complex<double>* dvecmult[iter];
  for(int i = 0; i < iter; i++) { 
    cout << "Reading ciphertext : " << i << " : ";
    cipher4[i] = SerializationUtils::readCiphertext("./MIL_ciphertexts/" + SFileName0 + std::to_string(i) + ".cip");
    cipher5[i] = SerializationUtils::readCiphertext("./MIL_ciphertexts/" + SFileName1 + std::to_string(i) + ".cip");
    cout << "DONE" << endl;

    cout << "Ciphertext mult : " << i << " : ";
    scheme.mult(cipherMult[i], *cipher4[i], *cipher5[i]);
    cout << "DONE" << endl;

    cout << "Decrypt : " << i << " : ";
    dvecmult[i] = scheme.decrypt(secretKey, cipherMult[i]);
    cout << "DONE" << endl;

  }
  }

  // Add decrypted //
  if(string(argv[1]) == "decadd") {
  Ciphertext* cipher6[iter];
  Ciphertext* cipher7[iter];
  complex<double>* dvec0[iter];
  complex<double>* dvec1[iter];
  complex<double>* dvecadd[iter];
  for(int i = 0; i < iter; i++) { 
    cout << "Reading ciphertext : " << i << " : ";
    cipher6[i] = SerializationUtils::readCiphertext("./MIL_ciphertexts/" + SFileName0 + std::to_string(i) + ".cip");
    cipher7[i] = SerializationUtils::readCiphertext("./MIL_ciphertexts/" + SFileName1 + std::to_string(i) + ".cip");
    cout << "DONE" << endl;

    cout << "Ciphertext decrypt : " << i << " : ";
    dvec0[i] = scheme.decrypt(secretKey, *cipher6[i]);
    dvec1[i] = scheme.decrypt(secretKey, *cipher7[i]);
    cout << "DONE" << endl;

    cout << "Decrypted add : " << i << " : ";
    complex<double>* dvectemp = new complex<double>[1024];
    for(int j = 0; j < 1024; j++) {
      dvectemp[j] = dvec0[i][j] + dvec1[i][j];
      dvecadd[i] = dvectemp;
    }
    cout << "DONE" << endl;

  }
  }
  
  // Multiply decrypted //
  if(string(argv[1]) == "decmult") {
  Ciphertext* cipher8[iter];
  Ciphertext* cipher9[iter];
  complex<double>* dvec2[iter];
  complex<double>* dvec3[iter];
  complex<double>* dvecmult[iter];
  for(int i = 0; i < iter; i++) { 
    cout << "Reading ciphertext : " << i << " : ";
    cipher8[i] = SerializationUtils::readCiphertext("./MIL_ciphertexts/" + SFileName0 + std::to_string(i) + ".cip");
    cipher9[i] = SerializationUtils::readCiphertext("./MIL_ciphertexts/" + SFileName1 + std::to_string(i) + ".cip");
    cout << "DONE" << endl;

    cout << "Ciphertext decrypt : " << i << " : ";
    dvec2[i] = scheme.decrypt(secretKey, *cipher8[i]);
    dvec3[i] = scheme.decrypt(secretKey, *cipher9[i]);
    cout << "DONE" << endl;

    cout << "Decrypted mult : " << i << " : ";
    complex<double>* dvectemp = new complex<double>[1024];
    for(int j = 0; j < 1024; j++) {
      dvectemp[j] = dvec2[i][j] * dvec3[i][j];
      dvecmult[i] = dvectemp;
    }
    cout << "DONE" << endl;

  }
  }

  /*
  // Multiplication And Rescale //
  Ciphertext cipherMult;
  scheme.mult(cipherMult, *cipher2, *cipher3);
  Ciphertext cipherMultAfterReScale;
  scheme.reScaleBy(cipherMultAfterReScale, cipherMult, logp);
  
  // Rotation //
  long idx = 1;
  Ciphertext cipherRot;
  scheme.leftRotateFast(cipherRot, cipher1, idx);
  */
  // Decrypt //
  /*
  cout << "Decrypt" << endl;
  complex<double>* dvec0 = scheme.decrypt(secretKey, *cipher2);
  complex<double>* dvec1 = scheme.decrypt(secretKey, *cipher3);
  cout << "DONE" << endl;

  complex<double> dvecadd[heaan::N];
  for(int i = 0; i < heaan::N; i++) {
    dvecadd[i] = dvec0[i] + dvec1[i];
  }
  */

  return 0;

}

std::complex<double>* loadDouble(const std::string FileName) {
    
    std::ifstream inFile(FileName);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for loading doubles.");
    }
    //complex<double>* loaded = new complex<double>[n];
    //std::vector<std::complex<double>> loadedVector;
    std::complex<double>* loadedArray = new std::complex<double>[1024];
    int i = 0;
    complex<double> number;
    while (inFile >> number) {
      // Create a complex number with the integer as the real part and 0 as the imaginary part
      //std::complex<double> complexNumber(number, 0);
      complex<double> complexNumber = number;

      // Add the complex number to the array
      loadedArray[i] = complexNumber;
      i++;
      //loadedVector.push_back(complexNumber);
    }
    
  inFile.close();
  return loadedArray;
  //return &loadedVector[0];
  
  return 0;
}

