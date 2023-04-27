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

using namespace std;

int main()
{   
    auto started = chrono::high_resolution_clock::now();

    int max = 10000000; // für höhere werte wird der double type benötigt (nicht implementiert bei avx wegen start verringerter leistung)
    int concurrency = thread::hardware_concurrency();
    concurrency = 4;

    vector<int> primes = {2};
    // vector<int> primes = {2, 3, 5, 7, 11};
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
        // lock funktioniert nicht so wie gedacht. rip. braucht nen kompletten rewrite.
        // arbeite dran.
        vector<thread*> threads;
        threads.reserve(concurrency);
        vector<int> status(concurrency);
        vector<vector<int>> outputs(concurrency);
        vector<mutex> mutexOutputs(concurrency);

        // Thread Status Table
        // 1: Working
        // 2: Completed
        // 3: Queued for Waiting
        // 4: Waiting

        // cout << "Starting Threads" << endl;

        for (size_t i = 0; i < concurrency; i++)
        {
            int threadNumber = i;
            status[i] = 1;
            #ifdef __AVX__
            threads[i] = thread::thread(checkPrimeThreaded, primes, ref(inputs[i]), ref(outputs[i]), status, threadNumber); //fix avx function (inverse primes parameter)
            #else
            thread* t = new thread(checkPrimeThreaded, ref(primes), ref(outputs[i]), ref(mutexOutputs[i]), ref(status), threadNumber, concurrency, max);
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
        

        bool completed = false;
        while (!completed)
        {   
            // cout << "Checking if completed" << endl;
            completed = true;
            for (size_t i = 0; i < status.size(); i++)
            {
                if (status[i] < max)
                {
                    completed = false;
                    break;
                }
            }
            
            // zahlen aus den outputs in den primzahl vektor geordnen einfügen
            // cout << "Reading Outputs" << endl;
            for (size_t i = 0; i < concurrency; i++)
            {
                // cout << "Checking Output" << endl;
                if (!outputs[i].empty())
                {
                    // cout << "Locking Output" << endl;
                    lock_guard<mutex> lock(mutexOutputs[i]);
                    // cout << "Expanding Result" << endl;
                    result.reserve(result.size() + outputs[i].size());
                    // cout << "Reading Output" << endl;
                    result.insert(result.end(), outputs[i].begin(), outputs[i].end());
                    // cout << "Clearing Output" << endl;
                    outputs[i].clear();
                    // largestPrimes[i] = results[i][results[i].size() - 1];
                }
                // if (!results[i].empty())
                // {
                //     // cout << "Logging Largest Prime" << endl;
                //     largestPrimes[i] = results[i][results[i].size() - 1];
                //     // cout << "Saved Largest Prime" << endl;
                // }
                
            }

            // cout << "Finding Smallest Element" << endl;
            // int smallestLargestIndex = distance(la.begin(), min_element(largestPrimes.begin(), largestPrimes.end()));
            // int smallestLargestPrime = largestPrimes[smallestLargestIndex];
            // int smallestSearched = status[distance(status.begin(), min_element(status.begin(), status.end()))];
            int smallestSearched = INT_MAX;
            for (size_t i = 0; i < status.size(); i++)
            {
                int value = status[i];
                if (value < smallestSearched)
                {
                    smallestSearched = value;
                }
            }
            // if (smallestSearched == INT_MAX)
            // {
            //     smallestSearched = -1;
            // }
            

            // cout << "smalles prime " << smallestLargestPrime << endl;
            
            // cout << "Splicing Vectors" << endl;
            // for (size_t i = 0; i < concurrency; i++)
            // {   
            //     if (!results[i].empty())
            //     {
            //         vector<int>::iterator lastElement = upper_bound(results[i].begin(), results[i].end(), smallestLargestPrime);
            //         if (lastElement <= results[i].begin())
            //             continue;
            //         result.insert(result.end(), results[i].begin(), lastElement);
            //         results[i].erase(results[i].begin(), lastElement);
            //     }
            // }
            
            // cout << "Sorting Result" << endl;
            

            if (!result.empty())
            {
                // vector<int> newPrimes (primes.size() + result.size());
                // #ifdef __AVX__
                // vector<float> newInversePrimes (newPrimes.size());
                // #endif

                sort(result.begin(), result.end());
                
                // merge(primes.begin(), primes.end(), result.begin(), result.end(), newPrimes.begin());
                // swap(primes, newPrimes);
                // result.clear();
                vector<int>::iterator lastElement = upper_bound(result.begin(), result.end(), smallestSearched);
                if (lastElement == result.begin())
                {
                    continue;
                }
                vector<int> newPrimes(primes.size() + distance(result.begin(), lastElement));
                // primes.insert(primes.end(), result.begin(), lastElement);
                merge(primes.begin(), primes.end(), result.begin(), lastElement, newPrimes.begin());
                result.erase(result.begin(), lastElement);

                // for (size_t i = 0; i < concurrency; i++)
                // {
                //     status[i] = 3;
                // }

                // bool waiting;
                // do
                // {
                //     waiting = true;
                //     for (size_t i = 0; i < concurrency; i++)
                //     {
                //         if (status[i] != 4)
                //         {
                //             waiting = false;
                //             break;
                //         }
                //     }
                    
                // } while (!waiting);
                
                swap(primes, newPrimes);

                // for (size_t i = 0; i < concurrency; i++)
                // {
                //     status[i] = 1;
                // }

                // #ifdef __AVX__
                // for (size_t i = 0; i < primes.size(); i++)
                // {
                //     newInversePrimes.push_back(1/primes[i]);
                // }
                // swap(inversePrimes, newInversePrimes);
                // #endif
            }
            
            // for (size_t j = 0; j < outputs.size(); j++)
            // {   unique_lock lock(mutexOutputs[j]);
            //     while (!outputs[j].empty())
            //     {
            //         int number = outputs[j].front();
            //         outputs[j].pop();
            //         results.push_back(number);
            //     }
            //     lock.unlock();
            // }

            // sort(results.begin(), results.end());
            // #ifdef __AVX__
            // vector<int> inverseResults(results.size());
            // for (size_t j = 0; j < results.size(); j++)
            // {
            //     inverseResults.push_back(1.0f/results[j]);
            // }
            // #endif

            // unique_lock lock1(primes);
            // #ifdef __AVX__
            // unique_lock lock2(inversePrimes);
            // #endif

            // cout << "Adding Primes" << endl;
            // primes.reserve(primes.size() + result.size());
            // primes.insert(primes.end(), result.begin(), result.end());
            // #ifdef __AVX__
            // inversePrimes.reserve(inversePrimes.size() + inverseResults.size());
            // inversePrimes.insert(inversePrimes.end(), inverseResults.begin(), inverseResults.end());
            // #endif
            
            // cout << "Cleanup and update" << endl;
                // cout << "notifying all" << endl;
                // sync.notify_all();
            // cout << "Done" << endl;
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