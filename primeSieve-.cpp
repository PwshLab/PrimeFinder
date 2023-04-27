#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <math.h>
#include <immintrin.h>

using namespace std;

bool checkPrime(vector<int>& primes, int number)
    {   
        if ((number % 2 == 0 && number != 2) || (number % 3 == 0 && number != 3) )
            return false;

        int v1;
        float v2;
        for (size_t i = 0; i < primes.size(); i++)
        {   
            v1 = primes[i];
            v2 = (float)number / v1;
            if (floorf(v2) == v2)
            {
                return false;
            }

            if (v1*v1 >= number)
            {
                return true;
            }
        }
    }

bool avxCheckPrime(vector<int>& primes, vector<float>& inversePrimes, int number)
{
    if ((number % 2 == 0 && number != 2) || (number % 3 == 0 && number != 3) )
        return false;
    
    array<int, 8> results = {1, 1, 1, 1, 1, 1, 1, 1};

    // https://stackoverflow.com/questions/46790237/vectorization-of-modulo-multiplication
    __m256 numVecF = _mm256_set1_ps((float)number);
    __m256i numVecI = _mm256_set1_epi32(number);
    
    int offsetPosition = 0;
    while (offsetPosition + 8 <= primes.size())
    {   
        __m256i prime = _mm256_loadu_si256((__m256i*)(primes.data() + offsetPosition));
        __m256 invPrime = _mm256_loadu_ps((float*)(inversePrimes.data() + offsetPosition));

        __m256i v1 = _mm256_cvtps_epi32( _mm256_mul_ps(numVecF, invPrime) );
        __m256i v2 = _mm256_sub_epi32(numVecI, _mm256_mullo_epi32( v1, prime) );

        _mm256_storeu_si256((__m256i*)(results.data()), v2);

        for (size_t i = 0; i < results.size(); i++)
        {
            if (results[i] == 0)
            {   
                return false;
            }
        }

        if (primes[offsetPosition]*primes[offsetPosition] >= number)
        {
            return true;
        }

        offsetPosition += 8;
    }

    if (offsetPosition == primes.size())
        return true;

    vector<int>::const_iterator newStart = primes.begin() + offsetPosition;
    vector<int>::const_iterator newEnd = primes.end();
    vector<int> extraPrimes(newStart, newEnd);

    return checkPrime(extraPrimes, number);
}

struct NumberPair
{
    int int1;
    int int2;
    NumberPair(int i1, int i2) {
        int1 = i1;
        int2 = i2;
    }
};


int main()
{   
    auto started = chrono::high_resolution_clock::now();

    int max = 10000000;
    vector<int> primes = {2};
    vector<float> inversePrimes = {1.0f/2};
    vector<NumberPair> primePairs = {};

    // https://de.wikipedia.org/wiki/Primzahlsatz
    primes.reserve( ceilf( ((float)max / log(max)) * 1.2f ) );
    inversePrimes.reserve( primes.size());
    primePairs.reserve( ceilf( (float)primes.size() * 0.1f ) );
    
    int i = 3;
    int lastPrime = 3;
    while (i < max)
    {   
        //if (checkPrime(primes, i))
        if (avxCheckPrime(primes, inversePrimes, i))
        {   
            primes.push_back(i);
            inversePrimes.push_back(1.0f/i);
            if (lastPrime + 2 == i)
            {
                primePairs.push_back(NumberPair(lastPrime, i));
            }
            lastPrime = i;
            //cout << i << " " << primes.size() << endl;
        }
        i += 2;
    }

    auto ended = chrono::high_resolution_clock::now();

    // for (size_t i = 0; i < primes.size(); i++)
    // {
    //     cout << primes[i] << ";";
    // }

    // cout << primes[primes.size() - 1] << endl << primes.size() << endl;
    
    for (size_t i = 0; i < primePairs.size(); i++)
    {
        cout << i << " " <<  primePairs[i].int1 << " " << primePairs[i].int2 << endl;
    }
    
    cout << primes.size() << endl;
    cout << primePairs.size() << endl;

    cout << chrono::duration_cast<chrono::milliseconds>(ended-started).count() << "ms" << endl;
}