#ifndef CHECKPRIME_H
#define CHECKPRIME_H

#include <vector>

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

bool checkPrime(std::vector<uint64_t>& primes, std::vector<uint64_t>& inversePrimes, uint32_t number);

bool checkPrimeAVX(std::vector<uint64_t>& primes, std::vector<uint64_t>& inversePrimes, uint32_t number);

#endif