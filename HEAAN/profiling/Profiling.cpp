#include "Profiling.h"

using namespace std;
using namespace NTL;
using namespace heaan;

int loadCiphertext(string FileName, Ciphertext &ciphertext);
std::complex<double>* loadInt(std::string FileName);
std::complex<double>* loadDouble(const std::string FileName);

int saveCiphertext(Ciphertext &ciphertext, std::string FileName);

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

  cout << "loading double" << endl; 
  complex<double>* mvec0 = loadDouble("randint_1024_0.txt");
  complex<double>* mvec1 = loadDouble("randint_1024_1.txt");
  cout << "DONE" << endl;
   
  for(int a = 0; a < n; a++) {
    cout << mvec0[a].real() << " + " << mvec0[a].imag() << "i" << endl;
  }
  for(int a = 0; a < n; a++) {
    cout << mvec1[a].real() << " + " << mvec1[a].imag() << "i" << endl;
  }
  
  // Encrypt Two Arry of Complex //
  Ciphertext cipher0;
  scheme.encrypt(cipher0, mvec0, n, logp, logq);
  Ciphertext cipher1;
  scheme.encrypt(cipher1, mvec1, n, logp, logq);
  
  cout << "saving randint" << endl;
  saveCiphertext(cipher0, "randint_cipher_1024_0.cip");
  cout << "0 DONE" << endl;
  saveCiphertext(cipher1, "randint_cipher_1024_1.cip");
  cout << "1 DONE" << endl;
  
  // Load ciphertexts //
  Ciphertext cipher2, cipher3;
  loadCiphertext("randint_cipher_1024_0.cip", cipher2);
  loadCiphertext("randint_cipher_1024_0.cip", cipher3);
  
  // Addition //
  cout << "Cipher add" << endl;
  Ciphertext cipherAdd;
  scheme.add(cipherAdd, cipher0, cipher1);
  cout << "0, 1 DONE" << endl;

  Ciphertext cipherAdd2;
  scheme.add(cipherAdd2, cipher2, cipher3);
  cout << "2, 3 DONE" << endl;

  
  // Multiplication And Rescale //
  Ciphertext cipherMult;
  scheme.mult(cipherMult, cipher1, cipher2);
  Ciphertext cipherMultAfterReScale;
  scheme.reScaleBy(cipherMultAfterReScale, cipherMult, logp);
  /*
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

int loadCiphertext(string FileName, Ciphertext &ciphertext) {
    std::ifstream inFile(FileName, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for loading ciphertext.");
    }

    // Read ciphertext parameters
    inFile.read(reinterpret_cast<char*>(&ciphertext.logp), sizeof(long));
    inFile.read(reinterpret_cast<char*>(&ciphertext.logq), sizeof(long)); 
    inFile.read(reinterpret_cast<char*>(&ciphertext.n), sizeof(long));

    // Resize the underlying vectors - Impossible with current array implementation.
    //ciphertext.ax.resize(ciphertext.n);
    //ciphertext.bx.resize(ciphertext.n);

    // Read ciphertext values

    //for (auto& coeff : ciphertext.ax) {
    for (int i = 0; i < ciphertext.n; i++) {
        inFile.read(reinterpret_cast<char*>(&(ciphertext.ax[i])), sizeof(uint64_t));
    }
    //for (auto& coeff : ciphertext.bx) {
    for (int i = 0; i < ciphertext.n; i++) {
        inFile.read(reinterpret_cast<char*>(&(ciphertext.ax[i])), sizeof(uint64_t));
    }

    inFile.close();
    
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

int saveCiphertext(Ciphertext &ciphertext, std::string FileName) {
    
    std::ofstream outFile(FileName, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for saving ciphertext.");
    }
    outFile.write(reinterpret_cast<const char*>(&ciphertext.logp), sizeof(long));
    outFile.write(reinterpret_cast<const char*>(&ciphertext.logq), sizeof(long));
    outFile.write(reinterpret_cast<const char*>(&ciphertext.n), sizeof(long));

    //for (const auto& coeff : &ciphertext.ax) {
    for (int i = 0; i < ciphertext.n; i++) {
        outFile.write(reinterpret_cast<const char*>(&(ciphertext.ax[i])), sizeof(uint64_t));
    }
    /*for (const auto& coeff : &ciphertext.bx) {
        outFile.write(reinterpret_cast<const char*>(&coeff), sizeof(uint64_t));
    }*/
    for (int i = 0; i < ciphertext.n; i++) {
        outFile.write(reinterpret_cast<const char*>(&(ciphertext.bx[i])), sizeof(uint64_t));
    }

    outFile.close();
    
    return 0;
}
