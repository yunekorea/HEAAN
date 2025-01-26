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

  //vector<complex<double>>* mvec0 = loadDouble("randint_1gb_0.txt");
  //vector<complex<double>>* mvec1 = loadDouble("randint_1gb_1.txt");
  //complex<double>* mvec0 = loadDouble(filename_0);
  //complex<double>* mvec1 = loadDouble(filename_1);
  
  complex<double>* mvec0 = loadDouble("randint_1gb_0.txt");
  complex<double>* mvec1 = loadDouble("randint_1gb_1.txt");

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

int loadCiphertext(string FileName, Ciphertext &ciphertext) {
    /* 
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for loading ciphertext.");
    }

    // Read ciphertext parameters
    inFile.read(reinterpret_cast<char*>(&ciphertext.logp), sizeof(long));
    inFile.read(reinterpret_cast<char*>(&ciphertext.logq), sizeof(long)); 
    inFile.read(reinterpret_cast<char*>(&ciphertext.n), sizeof(long));

    // Resize the underlying vectors
    ciphertext.ax.resize(ciphertext.n);
    ciphertext.bx.resize(ciphertext.n);

    // Read ciphertext values
    for (auto& coeff : ciphertext.ax) {
        inFile.read(reinterpret_cast<char*>(&coeff), sizeof(uint64_t));
    }
    for (auto& coeff : ciphertext.bx) {
        inFile.read(reinterpret_cast<char*>(&coeff), sizeof(uint64_t));
    }

    inFile.close();
    */
    return 0;
}

std::complex<double>* loadDouble(const std::string FileName) {
    
    std::ifstream inFile(FileName);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for loading doubles.");
    }
    //complex<double>* loaded = new complex<double>[n];
    std::vector<std::complex<double>> loadedVector;
    complex<double> number;
    while (inFile >> number) {
        // Create a complex number with the integer as the real part and 0 as the imaginary part
        //std::complex<double> complexNumber(number, 0);
        complex<double> complexNumber = number;

        // Add the complex number to the array
        loadedVector.push_back(complexNumber);
    }

    return &loadedVector[0];
    
    return 0;
}

int saveCiphertext(Ciphertext &ciphertext, std::string FileName) {
    /*
    std::ofstream outFile("integer1.cip", std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for saving ciphertext.");
    }
    outFile.write(reinterpret_cast<const char*>(&ciphertext.logp), sizeof(long));
    outFile.write(reinterpret_cast<const char*>(&ciphertext.logq), sizeof(long));
    outFile.write(reinterpret_cast<const char*>(&ciphertext.n), sizeof(long));

     for (const auto& coeff : ciphertext.ax) {
        outFile.write(reinterpret_cast<const char*>(&coeff), sizeof(uint64_t));
    }
    for (const auto& coeff : ciphertext.bx) {
        outFile.write(reinterpret_cast<const char*>(&coeff), sizeof(uint64_t));
    }

    outFile.close();
    */
    return 0;
}