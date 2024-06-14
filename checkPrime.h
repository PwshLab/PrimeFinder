#ifndef CHECKPRIME_H
#define CHECKPRIME_H

#include <vector>

bool checkPrime(vector<uint32_t>& primes, vector<uint32_t>& primesInverse, uint32_t number);

bool checkPrimeAVX(std::vector<uint32_t>& primes, std::vector<uint32_t>& primesInverse, uint32_t number);

#endif