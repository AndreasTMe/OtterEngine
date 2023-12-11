#ifndef OTTERENGINE_HASHUTILS_H
#define OTTERENGINE_HASHUTILS_H

#include "Core/BaseTypes.h"

namespace Otter::Internal
{
    class HashUtils final
    {
    public:
        [[nodiscard]] static UInt64 GetNextPrime(UInt64 value);
        [[nodiscard]] static UInt64 GetPreviousPrime(UInt64 value);

    private:
        HashUtils() = default;
        ~HashUtils() = default;
    };
}

#endif //OTTERENGINE_HASHUTILS_H