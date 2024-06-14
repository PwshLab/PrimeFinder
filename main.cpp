#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>

#include "checkPrime.h"

using namespace std;

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

uint32_t getReciprocal(uint32_t number)
{
    return UINT32_MAX / number + 1;
}

int main()
{   
    auto started = chrono::high_resolution_clock::now();

    const uint32_t max = pow(2, 24); // 16777216
    vector<uint64_t> primes = {2};
    vector<uint64_t> inversePrimes = {getReciprocal(2)};

    // https://de.wikipedia.org/wiki/Primzahlsatz
    const int estimate = ceilf( ((float)max / log(max)) * 1.1f ); // 10% korrektur nach oben zur sicherheit (spart 1ms bei ausfürung)
    primes.reserve(estimate);
    inversePrimes.reserve(estimate);
    
    for (size_t i = 3; i < max; i += 2)
    {   
        if (checkPrimeAVX(primes, inversePrimes, i))
        {   
            primes.push_back(i);
            inversePrimes.push_back(getReciprocal(i));
        }
    }
    
    auto ended = chrono::high_resolution_clock::now();
    
    cout << endl << "Primzahlen bis " << max << endl << endl;

    cout << primes.size() << " Primzahlen gefunden  (" << estimate << " erwartet)" << endl;

    cout << chrono::duration_cast<chrono::milliseconds>(ended-started).count() << "ms Bearbeitungszeit" << endl;
}