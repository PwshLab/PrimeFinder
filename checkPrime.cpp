#include <vector>
#include <array>
#include <math.h>
#include <immintrin.h>

using namespace std;

bool checkPrime(vector<uint32_t>& primes, vector<uint32_t>& inversePrimes, uint32_t number)
{   
    if ((number % 2 == 0 && number != 2) || (number % 3 == 0 && number != 3))
        return false;

    uint32_t comparisonValue;
    for (size_t i = 0; i < inversePrimes.size(); i++)
    {   
        comparisonValue = (((uint64_t)number * inversePrimes[i]) >> 32) * primes[i];
        if (comparisonValue == number)
        {
            return false;
        }

        if (primes[i] * primes[i] >= number)
        {
            return true;
        }
    }

    return false;
}

// https://stackoverflow.com/questions/46790237/vectorization-of-modulo-multiplication

bool checkPrimeAVX(vector<uint64_t>& primes, vector<uint64_t>& inversePrimes, uint32_t number)
{
    if ((number % 2 == 0 && number != 2) || (number % 3 == 0 && number != 3) )
        return false;

    array<uint64_t, 4> results = {1, 1, 1, 1};
    
    __m256i originalNumberVector = _mm256_set1_epi64x(number);

    int offsetPosition = 0;
    while (offsetPosition + 4 <= primes.size())
    {
        __m256i primesVector = _mm256_lddqu_si256((__m256i*)(primes.data() + offsetPosition));
        __m256i inversePrimesVector = _mm256_lddqu_si256((__m256i*)(inversePrimes.data() + offsetPosition));

        __m256i comparisonVector = _mm256_mul
    }
}

// bool checkPrimeAVXOld(vector<int>& primes, vector<int>& inversePrimes, int number)
// {
//     if ((number % 2 == 0 && number != 2) || (number % 3 == 0 && number != 3) )
//         return false;
    
//     array<int, 8> results = {1, 1, 1, 1, 1, 1, 1, 1};

//     __m256 numVecF = _mm256_set1_ps((float)number);
//     __m256i numVecI = _mm256_set1_epi32(number);
    
//     int offsetPosition = 0;
//     while (offsetPosition + 8 <= primes.size())
//     {   
//         __m256i prime = _mm256_loadu_si256((__m256i*)(primes.data() + offsetPosition));
//         __m256 invPrime = _mm256_loadu_ps((float*)(inversePrimes.data() + offsetPosition));

//         __m256i v1 = _mm256_cvtps_epi32( _mm256_mul_ps(numVecF, invPrime) );
//         __m256i v2 = _mm256_sub_epi32(numVecI, _mm256_mullo_epi32( v1, prime) );

//         _mm256_storeu_si256((__m256i*)(results.data()), v2);

//         for (size_t i = 0; i < results.size(); i++)
//         {
//             if (results[i] == 0)
//             {   
//                 return false;
//             }
//         }

//         if (primes[offsetPosition]*primes[offsetPosition] > number)
//         {
//             return true;
//         }

//         offsetPosition += 8;
//     }

//     if (offsetPosition == primes.size())
//         return true;

//     vector<int>::const_iterator newStart = primes.begin() + offsetPosition;
//     vector<int>::const_iterator newEnd = primes.end();
//     vector<int> extraPrimes(newStart, newEnd);

//     return checkPrimeF(extraPrimes, number);
// }

