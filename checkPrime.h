#ifndef CHECKPRIME_H
#define CHECKPRIME_H

#include <vector>

bool checkPrimeF(std::vector<int>& primes, int number);
bool checkPrimeD(std::vector<int>& primes, int number);

#ifdef __AVX__
bool checkPrimeAVX(std::vector<int>& primes, std::vector<float>& inversePrimes, int number);
#endif

#endif