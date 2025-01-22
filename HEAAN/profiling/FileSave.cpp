#include "FileSave.h"

using namespace std;
using namespace NTL;

namespace heaan {

int saveCiphertext(Ciphertext &ciphertext, string FileName) {
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

    return 0;
}

}