#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "checkPrime.h"

#include <iostream>

using namespace std;

void checkPrimeThreaded(vector<int>& primes, vector<int>& output, mutex& outputMutex, vector<int>& status, const int threadNumber, const int threadCount, const int max)
{
    for (size_t number = 3 + threadNumber * 2; number < max; number += threadCount * 2)
    {   
        status[threadNumber] = number;
        // if (status[threadNumber] == 3)
        // {
        //     status[threadNumber] = 4;
        //     while (status[threadNumber] != 1)
        //     { 
        //     }
        // }
        if (number > primes.back() * primes.back())
        {
            // status[threadNumber] = 4;
            while (number > primes.back() * primes.back())
            {
            }
            // status[threadNumber] = 1;
        }

        bool prime = checkPrimeF(primes, number);
        
        if (prime)
        {
            lock_guard<mutex> lock(outputMutex);
            output.push_back(number);
        }
    }
    status[threadNumber] = max + 1;
    cout << "Thread " << threadNumber << " is exiting" << endl;
}