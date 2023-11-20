#ifndef OTTERENGINE_HASHUTILS_H
#define OTTERENGINE_HASHUTILS_H

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Otter::Internal
{
    class HashUtils final
    {
    public:
        [[nodiscard]] static UInt64 GetNextPrime(UInt64 value);
        [[nodiscard]] static UInt64 GetPreviousPrime(UInt64 value);

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(HashUtils)
    };
}

#endif //OTTERENGINE_HASHUTILS_H