#include "FileLoad.h"

using namespace std;
using namespace NTL;

namespace heaan {

int loadCiphertext(string FileName, Ciphertext &ciphertext) {
    
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
    
    return 0;
}

complex<double>* loadInt(string FileName) {
    std::ifstream inFile(filename);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for loading integers.");
    }

    return 0;
}

std::vector<std::complex<double>>* loadDouble(string FileName) {
    std::ifstream inFile(FileName);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for loading doubles.");
    }
    //complex<double>* loaded = new complex<double>[n];
    std::vector<std::complex<double>> loadedVector;
    complex<double> number;
    while (inputFile >> number) {
        // Create a complex number with the integer as the real part and 0 as the imaginary part
        //std::complex<double> complexNumber(number, 0);
        complex<double> complexNumber = number;

        // Add the complex number to the array
        loadedVector.push_back(complexNumber);
    }

    return loadedVector;
    //return 0;
}

}