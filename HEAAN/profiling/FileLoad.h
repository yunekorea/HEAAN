#ifndef PROF_FILELOAD_H_
#define PROF_FILELOAD_H_

#include "../src/HEAAN.h"
#include <fstream>

int loadCiphertext(std::string FileName, heaan::Ciphertext &ciphertext);
std::vector<std::complex<double>>* loadInt(std::string FileName);
std::vector<std::complex<double>>* loadDouble(std::string FileName);

#endif