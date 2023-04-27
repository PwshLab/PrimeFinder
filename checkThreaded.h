#ifndef CHECKTHREADED_H
#define CHECKTHREADED_H

#include <vector>
#include <mutex>
#include <condition_variable>

void checkPrimeThreaded(std::vector<int>& primes, std::vector<int>& output, std::mutex& outputMutex, std::vector<int>& status, const int threadNumber, const int threadCount, const int max);

#ifdef __AVX__

#endif

#endif