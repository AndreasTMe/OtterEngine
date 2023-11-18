#include "Otter.PCH.h"

#include "Core/Collections/Utils/HashUtils.h"

namespace Otter::Internal
{
    Span<UInt64, 50> g_Primes;

    void PopulatePrimes();

    UInt64 HashUtils::GetNextPrime(const UInt64 value)
    {
        if (g_Primes[0] == 0)
            PopulatePrimes();

        if (value <= g_Primes[0])
            return g_Primes[0];

        for (auto i = 1; i < g_Primes.Length(); i++)
            if (g_Primes[i] >= value)
                return g_Primes[i];

        OTR_LOG_WARNING("Prime number must be greater than {0}", value)

        return g_Primes[g_Primes.Length() - 1];
    }

    UInt64 HashUtils::GetPreviousPrime(const UInt64 value)
    {
        if (g_Primes[0] == 0)
            PopulatePrimes();

        for (auto i = g_Primes.Length() - 1; i > 0; i--)
            if (g_Primes[i] <= value)
                return g_Primes[i];

        return g_Primes[0];
    }

    void PopulatePrimes()
    {
        g_Primes[0]  = 3;
        g_Primes[1]  = 7;
        g_Primes[2]  = 11;
        g_Primes[3]  = 17;
        g_Primes[4]  = 23;
        g_Primes[5]  = 29;
        g_Primes[6]  = 37;
        g_Primes[7]  = 47;
        g_Primes[8]  = 59;
        g_Primes[9]  = 71;
        g_Primes[10] = 89;
        g_Primes[11] = 107;
        g_Primes[12] = 131;
        g_Primes[13] = 163;
        g_Primes[14] = 197;
        g_Primes[15] = 239;
        g_Primes[16] = 293;
        g_Primes[17] = 353;
        g_Primes[18] = 431;
        g_Primes[19] = 521;
        g_Primes[20] = 631;
        g_Primes[21] = 761;
        g_Primes[22] = 919;
        g_Primes[23] = 1103;
        g_Primes[24] = 1327;
        g_Primes[25] = 1597;
        g_Primes[26] = 1931;
        g_Primes[27] = 2333;
        g_Primes[28] = 2801;
        g_Primes[29] = 3371;
        g_Primes[30] = 4049;
        g_Primes[31] = 4861;
        g_Primes[32] = 5839;
        g_Primes[33] = 7013;
        g_Primes[34] = 8419;
        g_Primes[35] = 10103;
        g_Primes[36] = 12143;
        g_Primes[37] = 14591;
        g_Primes[38] = 17519;
        g_Primes[39] = 21023;
        g_Primes[40] = 25229;
        g_Primes[41] = 30293;
        g_Primes[42] = 36353;
        g_Primes[43] = 43627;
        g_Primes[44] = 52361;
        g_Primes[45] = 62851;
        g_Primes[46] = 75431;
        g_Primes[47] = 90523;
        g_Primes[48] = 108631;
        g_Primes[49] = 130363;
    }
}