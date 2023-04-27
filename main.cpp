#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>
#include <thread>
#include <mutex>
#include <algorithm>

#include "numberPair.h"
#include "checkPrime.h"
#include "checkThreaded.h"
#include "checkComs.h"

using namespace std;

int main()
{   
    auto started = chrono::high_resolution_clock::now();

    const int max = 10000000; // für höhere werte wird der double type benötigt (nicht implementiert bei avx wegen start verringerter leistung)
    int concurrency = thread::hardware_concurrency();

    vector<int> primes = {2};
    #ifdef __AVX__
    vector<float> inversePrimes = {1.0f/2};
    #endif
    vector<NumberPair> primePairs = {};

    // https://de.wikipedia.org/wiki/Primzahlsatz
    int estimate = ceilf( ((float)max / log(max)) * 1.1f ); // 10% korrektur nach oben zur sicherheit (spart 1ms bei ausfürung)
    primes.reserve(estimate);
    #ifdef __AVX__
    inversePrimes.reserve(estimate);
    #endif
    primePairs.reserve(estimate / 10);
    
    if (concurrency <= 2)
    {
        for (size_t i = 3; i < max; i += 2)
        {   
            #ifdef __AVX__
            if (checkPrimeAVX(primes, inversePrimes, i))
            #else
            if (checkPrimeF(primes, i))
            #endif
            {   
                primes.push_back(i);
                #ifdef __AVX__
                inversePrimes.push_back(1.0f/i);
                #endif
            }
        }
    }
    else
    {
        vector<thread*> threads;
        threads.reserve(concurrency);
        vector<CheckComs> coms(concurrency);

        for (size_t i = 0; i < concurrency; i++)
        {
            int threadNumber = i;
            #ifdef __AVX__
            threads[i] = thread::thread(checkPrimeThreaded, primes, ref(inputs[i]), ref(outputs[i]), status, threadNumber); //fix avx function (inverse primes parameter)
            #else
            thread* t = new thread(checkPrimeThreaded, ref(primes), ref(coms[threadNumber]));
            threads.push_back(t);
            #endif
        }

        // cout << "Started Threads" << endl;

        // vector<vector<int>> results(concurrency);
        vector<int> result;

        // for (size_t i = 0; i < concurrency; i++)
        // {
        //     largestPrimes[i] = INT_MAX;
        // }
        
        const int increment = 2;
        int minNumbersPerThread = concurrency * concurrency;
        int currentNumber = 3;

        while (currentNumber < max)
        {
            int currentMax = min(primes.back() * primes.back() - 1, max);
            int nextSize = currentMax - currentNumber;
            int offset = nextSize % concurrency;
            int nextBlockSize = (nextSize - offset) / concurrency;
            if (nextBlockSize < minNumbersPerThread)
            {
                for (; currentNumber < currentMax; currentNumber += increment)
                {
                    if (checkPrimeF(primes, currentNumber))
                    {
                        primes.push_back(currentNumber);
                    }
                }
            }
            else
            {
                for (size_t i = 0; i < concurrency; i++)
                {
                    int lastNumber = currentNumber + nextBlockSize;
                    coms[i].AddJob(CheckJob(currentNumber, lastNumber, increment));
                    currentNumber = lastNumber;
                }

                bool completed = false;
                while (!completed)
                {
                    completed = true;
                    for (size_t i = 0; i < concurrency; i++)
                    {
                        if (coms[i].IsWorking())
                        {
                            completed = false;
                            break;
                        }
                    }
                }

                vector<int> result;
                for (size_t i = 0; i < concurrency; i++)
                {
                    if (coms[i].HasResult())
                    {
                        coms[i].ReceiveResultUnsorted(result);
                    }
                }
                sort(result.begin(), result.end());

                primes.insert(primes.end(), result.begin(), result.end());
            }
        }
        
        // unique_lock lock(mutexStatus);
        // for (size_t i = 0; i < status.size(); i++)
        // {
        //     status[i] = 3;
        // }
        // lock.unlock();
        
        for (size_t i = 0; i < threads.size(); i++)
        {
            threads[i]->join();
        }

        // sort(results.begin(), results.end());
        // primes.reserve(primes.size() + results.size());
        // primes.insert(primes.end(), results.begin(), results.end());
    }

    for (size_t i = 0; i < primes.size() - 1; i++)
    {
        if (primes[i] + 2 == primes[i + 1])
        {
            primePairs.push_back(NumberPair(primes[i], primes[i + 1]));
        }
    }
    
    auto ended = chrono::high_resolution_clock::now();
    
    
    for (size_t i = 0; i < primePairs.size(); i++)
    {
        // cout << primePairs[i].int1 << " " << primePairs[i].int2 << endl;
    }
    
    cout << endl << "Primzahlen bis " << max << endl << endl;

    cout << primes.size() << " Primzahlen gefunden  (" << estimate << " erwartet)" << endl;
    cout << primePairs.size() << " Primzahlpaare gefunden" << endl;

    cout << chrono::duration_cast<chrono::milliseconds>(ended-started).count() << "ms Bearbeitungszeit" << endl;
}