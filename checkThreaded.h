#ifndef CHECKTHREADED_H
#define CHECKTHREADED_H

#include <vector>

#include "checkComs.h"

void checkPrimeThreaded(std::vector<int>& primes, CheckComs& communications);

#ifdef __AVX__

#endif

#endif