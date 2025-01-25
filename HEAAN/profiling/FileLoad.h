#ifndef PROF_FILELOAD_H_
#define PROF_FILELOAD_H_

#include "../src/HEAAN.h"
#include <fstream>
#include <string>

int loadCiphertext(std::string FileName, heaan::Ciphertext &ciphertext);
//std::vector<std::complex<double>>* loadInt(std::string FileName);
std::complex<double>* loadInt(std::string FileName);
//std::vector<std::complex<double>>* loadDouble(std::string FileName);
//std::complex<double>* loadDouble(const std::string FileName);
int loadDouble(int FileName);

#endif
