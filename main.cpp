#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>

#include "numberPair.h"
#include "checkPrime.h"

using namespace std;

int main()
{   
    auto started = chrono::high_resolution_clock::now();

    int max = 10000000; // für höhere werte wird der double type benötigt (nicht implementiert bei avx wegen start verringerter leistung)

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
        cout << primePairs[i].int1 << " " << primePairs[i].int2 << endl;
    }
    
    cout << endl << "Primzahlen bis " << max << endl << endl;

    cout << primes.size() << " Primzahlen gefunden  (" << estimate << " erwartet)" << endl;
    cout << primePairs.size() << " Primzahlpaare gefunden" << endl;

    cout << chrono::duration_cast<chrono::milliseconds>(ended-started).count() << "ms Bearbeitungszeit" << endl;
}