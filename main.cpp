#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>

#include "numberPair.h"
#include "checkPrime.h"

using namespace std;

uint32_t getReciprocal(uint32_t number)
{
    return UINT32_MAX / number + 1;
}

int main()
{   
    auto started = chrono::high_resolution_clock::now();

    const uint32_t max = pow(2, 24); // 16777216
    vector<uint32_t> primes = {2};
    vector<uint32_t> primesInverse = {getReciprocal(2)};

    // https://de.wikipedia.org/wiki/Primzahlsatz
    const int estimate = ceilf( ((float)max / log(max)) * 1.1f ); // 10% korrektur nach oben zur sicherheit (spart 1ms bei ausfürung)
    primes.reserve(estimate);
    primesInverse.reserve(estimate);
    
    for (size_t i = 3; i < max; i += 2)
    {   
        if (checkPrimeAVX(primes, primesInverse, i))
        {   
            primes.push_back(i);
            primesInverse.push_back(getReciprocal(i));
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