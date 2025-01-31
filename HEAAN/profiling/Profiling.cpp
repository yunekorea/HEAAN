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
  complex<double>* mvec0 = EvaluatorUtils::randomComplexArray(slots);
  complex<double>* mvec1 = EvaluatorUtils::randomComplexArray(slots);

  cout << "loading double" << endl; 
  //complex<double>* mvec0 = loadDouble("randint_1024_0.txt");
  //complex<double>* mvec1 = loadDouble("randint_1024_1.txt");
  cout << "DONE" << endl;

   
  /*
  for(int a = 0; a < n; a++) {
    cout << mvec0[a].real() << " + " << mvec0[a].imag() << "i" << endl;
  }
  for(int a = 0; a < n; a++) {
    cout << mvec1[a].real() << " + " << mvec1[a].imag() << "i" << endl;
  }
  */
  
  /*
  // Encrypt Two Arry of Complex //
  Ciphertext cipher0;
  scheme.encrypt(cipher0, mvec0, n, logp, logq);
  Ciphertext cipher1;
  scheme.encrypt(cipher1, mvec1, n, logp, logq);
  
  
  cout << "saving randint" << endl;
  SerializationUtils::writeCiphertext(cipher0, "randint_cipher_1024_0.cip");
  cout << "0 DONE" << endl;
  SerializationUtils::writeCiphertext(cipher1, "randint_cipher_1024_1.cip");
  cout << "1 DONE" << endl;
  */ 

  // Load ciphertexts //
  
  Ciphertext* cipher2;
  Ciphertext* cipher3;
  //loadCiphertext("randint_cipher_1024_0.cip", cipher2);
  //loadCiphertext("randint_cipher_1024_1.cip", cipher3);
  cipher2 = SerializationUtils::readCiphertext("randint_cipher_1024_0.cip");
  cipher3 = SerializationUtils::readCiphertext("randint_cipher_1024_1.cip");
  cout << "read DONE" << endl;
  /*
  for(int z = 0; z < cipher2.n; z++) {
    if(cipher0.ax[z] != cipher2.ax[z])
      cout << "Cipher difference" << endl;
    if(cipher0.bx[z] != cipher2.bx[z])
      cout << "Cipher difference" << endl;
    if(cipher1.ax[z] != cipher3.ax[z])
      cout << "Cipher difference" << endl;
    if(cipher1.bx[z] != cipher3.bx[z])
      cout << "Cipher difference" << endl;
  }
  *?
  
  
  /*
  // Addition //
  cout << "Cipher add" << endl;
  Ciphertext cipherAdd;
  scheme.add(cipherAdd, cipher0, cipher1);
  cout << "0, 1 DONE" << endl;
  */
  //cout << "cipher 2 : " << cipher2->logp << "; " << cipher2->logq << "; " << cipher2->n << endl;
  //cout << "cipher 3 : " << cipher3->logp << "; " << cipher3->logq << "; " << cipher3->n << endl;
  cout << "Cipher add" << endl;
  /*
  for(int z = 0; z < heaan::N; z++) {
    cout << cipher0.ax[z] << " " << cipher0.bx[z] << " " << cipher1.ax[z] << " " << cipher1.bx[z] << endl;
  }
  */

  Ciphertext cipherAdd2;
  scheme.add(cipherAdd2, *cipher2, *cipher3);
  cout << "2, 3 DONE" << endl;
  
  /*
  // Multiplication And Rescale //
  Ciphertext cipherMult;
  scheme.mult(cipherMult, cipher0, cipher1);
  Ciphertext cipherMultAfterReScale;
  scheme.reScaleBy(cipherMultAfterReScale, cipherMult, logp);
  
  // Rotation //
  long idx = 1;
  Ciphertext cipherRot;
  scheme.leftRotateFast(cipherRot, cipher1, idx);
  */
  // Decrypt //
  cout << "Decrypt" << endl;
  complex<double>* dvec0 = scheme.decrypt(secretKey, *cipher2);
  complex<double>* dvec1 = scheme.decrypt(secretKey, *cipher3);
  cout << "DONE" << endl;

  return 0;

}

int loadCiphertext(string FileName, Ciphertext &ciphertext) {
    std::ifstream inFile(FileName, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for loading ciphertext.");
    }

    // Read ciphertext parameters
    inFile.read((char*)&ciphertext.logp, sizeof(long));
    inFile.read((char*)&ciphertext.logq, sizeof(long)); 
    inFile.read((char*)&ciphertext.n, sizeof(long));

    // Ensure ax and bx are properly allocated
    //ciphertext.ax.resize(heaan::N);
    //ciphertext.bx.resize(heaan::N);

    // Read ciphertext values
    for (int i = 0; i < heaan::N; i++) {
        /*
        size_t ax_size;
        inFile.read((char*)&ax_size, sizeof(size_t)); // Read string size
        std::string ax_str(ax_size, '\0'); // Allocate string buffer
        inFile.read(&ax_str[0], ax_size);  // Read string data
        ciphertext.ax[i] = NTL::conv<NTL::ZZ>(ax_str); // Convert back to NTL::ZZ
        */
        inFile.read((char*)&ciphertext.ax[i], sizeof(NTL::ZZ));

    }
    for (int i = 0; i < heaan::N; i++) {
        /*
        size_t bx_size;
        inFile.read((char*)&bx_size, sizeof(size_t)); // Read string size
        std::string bx_str(bx_size, '\0'); // Allocate string buffer
        inFile.read(&bx_str[0], bx_size);  // Read string data
        ciphertext.bx[i] = NTL::conv<NTL::ZZ>(bx_str); // Convert back to NTL::ZZ
        */
        inFile.read((char*)&ciphertext.bx[i], sizeof(NTL::ZZ));
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
    outFile.write((char*)&ciphertext.logp, sizeof(long));
    outFile.write((char*)&ciphertext.logq, sizeof(long));
    outFile.write((char*)&ciphertext.n, sizeof(long));

    for (int i = 0; i < heaan::N; i++) {
      /*
      std::string ax_str = NTL::conv<std::string>(ciphertext.ax[i]);
      size_t ax_size = ax_str.size();
      outFile.write((char*)&ax_size, sizeof(size_t));
      outFile.write(ax_str.data(), ax_size);
      */
      outFile.write((char*)&ciphertext.ax[i], sizeof(NTL::ZZ));
    }
    
    for (int i = 0; i < heaan::N; i++) {
      /*
      std::string bx_str = NTL::conv<std::string>(ciphertext.bx[i]);
      size_t bx_size = bx_str.size();
      outFile.write((char*)&bx_size, sizeof(size_t));
      outFile.write(bx_str.data(), bx_size);
      */
      outFile.write((char*)&ciphertext.bx[i], sizeof(NTL::ZZ));
    }

    outFile.close();
    
    return 0;
}
