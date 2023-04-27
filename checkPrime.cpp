#include <vector>
#include <array>
#include <math.h>
#include <immintrin.h>

using namespace std;

bool checkPrimeF(vector<int>& primes, int number)
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

            if (v1*v1 > number)
            {
                return true;
            }
        }

        return false;
    }

bool checkPrimeD(vector<int>& primes, int number)
{   
    if ((number % 2 == 0 && number != 2) || (number % 3 == 0 && number != 3) )
        return false;

    int v1;
    double v2;
    for (size_t i = 0; i < primes.size(); i++)
    {   
        v1 = primes[i];
        v2 = (double)number / v1;
        if (floorl(v2) == v2)
        {
            return false;
        }

        if (v1*v1 > number)
        {
            return true;
        }
    }

    return false;
}

// https://stackoverflow.com/questions/46790237/vectorization-of-modulo-multiplication

#if defined(__AVX512F__)

bool checkPrimeAVX(vector<int>& primes, vector<float>& inversePrimes, int number)
{
    if ((number % 2 == 0 && number != 2) || (number % 3 == 0 && number != 3) )
        return false;
    
    array<int, 16> results = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    __m512 numVecF = _mm512_set1_ps((float)number);
    __m512i numVecI = _mm512_set1_epi32(number);


    int offsetPosition = 0;
    while (offsetPosition + 16 <= primes.size())
    {   
        __m512i prime = _mm512_loadu_si512((__m512i*)(primes.data() + offsetPosition));
        __m512 invPrime = _mm512_loadu_ps((float*)(inversePrimes.data() + offsetPosition));

        __m512i v1 = _mm512_cvtps_epi32( _mm512_mul_ps(numVecF, invPrime) );
        __m512i v2 = _mm512_sub_epi32(numVecI, _mm512_mullo_epi32( v1, prime ) );

        _mm512_storeu_si512((__m512i*)(results.data()), v2);

        for (size_t i = 0; i < results.size(); i++)
        {
            if (results[i] == 0)
            {   
                return false;
            }
        }

        if (primes[offsetPosition]*primes[offsetPosition] > number)
        {
            return true;
        }

        offsetPosition += 16;
    }

    if (offsetPosition == primes.size())
        return true;

    vector<int>::const_iterator newStart = primes.begin() + offsetPosition;
    vector<int>::const_iterator newEnd = primes.end();
    vector<int> extraPrimes(newStart, newEnd);

    return checkPrimeF(extraPrimes, number);
}

#elif defined(__AVX2__)

bool checkPrimeAVX(vector<int>& primes, vector<float>& inversePrimes, int number)
{
    if ((number % 2 == 0 && number != 2) || (number % 3 == 0 && number != 3) )
        return false;
    
    array<int, 8> results = {1, 1, 1, 1, 1, 1, 1, 1};

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

        if (primes[offsetPosition]*primes[offsetPosition] > number)
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

    return checkPrimeF(extraPrimes, number);
}

#elif defined(__AVX__)

bool checkPrimeAVX(vector<int>& primes, vector<float>& inversePrimes, int number)
{
    if ((number % 2 == 0 && number != 2) || (number % 3 == 0 && number != 3) )
        return false;
    
    array<int, 4> results = {1, 1, 1, 1};

    __m128 numVecF = _mm_set1_ps((float)number);
    __m128i numVecI = _mm_set1_epi32(number);
    
    int offsetPosition = 0;
    while (offsetPosition + 4 <= primes.size())
    {   
        __m128i prime = _mm_loadu_si128((__m128i*)(primes.data() + offsetPosition));
        __m128 invPrime = _mm_loadu_ps((float*)(inversePrimes.data() + offsetPosition));

        __m128i v1 = _mm_cvtps_epi32( _mm_mul_ps(numVecF, invPrimes) );
        __m128i v2 = _mm_sub_epi32(numVecI, _mm_mullo_epi32( v1, prime ) );

        _mm_storeu_si128((__m128i*)(results.data()), v2);

        for (size_t i = 0; i < results.size(); i++)
        {
            if (results[i] == 0)
            {   
                return false;
            }
        }

        if (primes[offsetPosition]*primes[offsetPosition] > number)
        {
            return true;
        }

        offsetPosition += 4;
    }

    if (offsetPosition == primes.size())
        return true;

    vector<int>::const_iterator newStart = primes.begin() + offsetPosition;
    vector<int>::const_iterator newEnd = primes.end();
    vector<int> extraPrimes(newStart, newEnd);

    return checkPrimeF(extraPrimes, number);
}

#endif