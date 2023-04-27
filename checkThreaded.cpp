#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "checkPrime.h"
#include "checkComs.h"

#include <iostream>

using namespace std;

void checkPrimeThreaded(vector<int>& primes, CheckComs& communications)
{
    // for (size_t number = 3 + threadNumber * 2; number < max; number += threadCount * 2)
    // {   
    //     status[threadNumber] = number;
    //     // if (status[threadNumber] == 3)
    //     // {
    //     //     status[threadNumber] = 4;
    //     //     while (status[threadNumber] != 1)
    //     //     { 
    //     //     }
    //     // }
    //     if (number > primes.back() * primes.back())
    //     {
    //         // status[threadNumber] = 4;
    //         while (number > primes.back() * primes.back())
    //         {
    //         }
    //         // status[threadNumber] = 1;
    //     }

    //     bool prime = checkPrimeF(primes, number);
        
    //     if (prime)
    //     {
    //         lock_guard<mutex> lock(outputMutex);
    //         output.push_back(number);
    //     }
    // }
    // status[threadNumber] = max + 1;
    // cout << "Thread " << threadNumber << " is exiting" << endl;
    while (!communications.ShouldExit())
    {
        if (!communications.HasJob())
        {
            continue;
        }
        CheckJob currentJob = communications.ReceiveJob();
        for (size_t number = currentJob.lower; number < currentJob.upper; number += currentJob.increment)
        {
            bool prime = checkPrimeF(primes, number);
            if (prime)
            {
                communications.AddResult(number);
            }
        }
        communications.StopWorking();
    }
}