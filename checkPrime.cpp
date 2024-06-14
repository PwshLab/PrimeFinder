#include <vector>
#include <array>
#include <math.h>
#include <immintrin.h>

using namespace std;

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

bool checkPrime(vector<uint64_t>& primes, vector<uint64_t>& inversePrimes, uint32_t number)
{   
    if ((number % 2 == 0 && number != 2) || (number % 3 == 0 && number != 3))
        return false;

    uint64_t comparisonValue;
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

// From https://stackoverflow.com/questions/37296289/fastest-way-to-multiply-an-array-of-int64-t
// replace hadd -> shuffle (4 uops) with shift/and/add (3 uops with less shuffle-port pressure)
// The constant takes 2 insns to generate outside a loop.
__m256i mul64_avx2 (__m256i a, __m256i b)
{
    // There is no vpmullq until AVX-512. Split into 32-bit multiplies
    // Given a and b composed of high<<32 | low  32-bit halves
    // a*b = a_low*(u64)b_low  + (u64)(a_high*b_low + a_low*b_high)<<32;  // same for signed or unsigned a,b since we aren't widening to 128
    // the a_high * b_high product isn't needed for non-widening; its place value is entirely outside the low 64 bits.

    __m256i b_swap  = _mm256_shuffle_epi32(b, _MM_SHUFFLE(2,3, 0,1));   // swap H<->L
    __m256i crossprod  = _mm256_mullo_epi32(a, b_swap);                 // 32-bit L*H and H*L cross-products

    __m256i prodlh = _mm256_slli_epi64(crossprod, 32);          // bring the low half up to the top of each 64-bit chunk 
    __m256i prodhl = _mm256_and_si256(crossprod, _mm256_set1_epi64x(0xFFFFFFFF00000000)); // isolate the other, also into the high half were it needs to eventually be
    __m256i sumcross = _mm256_add_epi32(prodlh, prodhl);       // the sum of the cross products, with the low half of each u64 being 0.

    __m256i prodll  = _mm256_mul_epu32(a,b);                  // widening 32x32 => 64-bit  low x low products
    __m256i prod    = _mm256_add_epi32(prodll, sumcross);     // add the cross products into the high half of the result
    return  prod;
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

        __m256i comparisonVector = _mm256_sub_epi64(mul64_avx2(_mm256_srli_epi64(mul64_avx2(originalNumberVector, inversePrimesVector), 32), primesVector), originalNumberVector);
        
        _mm256_storeu_si256((__m256i*)(results.data()), comparisonVector);

        for (size_t i = 0; i < results.size(); i++)
        {
            if (results[i] == 0)
            {
                return false;
            }
        }
        
        if (primes[offsetPosition] * primes[offsetPosition] > number)
        {
            return true;
        }

        offsetPosition += 4;
    }

    if (offsetPosition == primes.size())
        return true;

    vector<uint64_t> extraPrimes(primes.begin() + offsetPosition, primes.end());
    vector<uint64_t> extraInversePrimes(inversePrimes.begin() + offsetPosition, inversePrimes.end());

    return checkPrime(extraPrimes, extraInversePrimes, number);
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

