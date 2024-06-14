#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <math.h>
#include <immintrin.h>

using namespace std;

bool checkPrime(vector<uint32_t>& primes, uint32_t number)
{   
    if ((number % 2 == 0 && number != 2) || (number % 3 == 0 && number != 3))
        return false;

    uint32_t currentPrime;
    uint32_t comparisonValue;
    for (size_t i = 0; i < primes.size(); i++)
    {   
        currentPrime = primes[i];
        comparisonValue = (number / currentPrime) * currentPrime;
        if (comparisonValue == currentPrime)
        {
            return false;
        }

        if (currentPrime * currentPrime >= number)
        {
            return true;
        }
    }

    return false;
}

int main()
{   
    auto started = chrono::high_resolution_clock::now();

    const uint32_t max = pow(2, 24);
    vector<uint32_t> primes = {2};

    // https://de.wikipedia.org/wiki/Primzahlsatz
    int estimate = ceilf( ((float)max / log(max)) * 1.1f ); // 10% korrektur nach oben zur sicherheit
    primes.reserve(estimate);
    
    int i = 3;
    while (i < max)
    {   
        if (checkPrime(primes, i))
        {   
            primes.push_back(i);
        }
        i += 2;
    }

    auto ended = chrono::high_resolution_clock::now();
    
    cout << endl << "Primzahlen bis " << max << endl << endl;

    cout << primes.size() << " Primzahlen gefunden  (" << estimate << " erwartet)" << endl;

    cout << chrono::duration_cast<chrono::milliseconds>(ended-started).count() << "ms Bearbeitungszeit" << endl;

    // cout << endl << "Fuer bessere Performace im Compiler AVX und Optimierungen aktivieren (Optionen '-march=native -Ofast' bei GCC)" << endl;
}
