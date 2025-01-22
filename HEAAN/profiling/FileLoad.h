#ifndef PROF_FILELOAD_H_
#define PROF_FILELOAD_H_

#include "../src/HEAAN.h"
#include <fstream>

int loadCiphertext(string FileName, Ciphertext &ciphertext);
std::vector<std::complex<double>>* loadInt(string FileName, Ciphertext &ciphertext);
std::vector<std::complex<double>>* loadDouble(string FileName, Ciphertext &ciphertext);

#endif