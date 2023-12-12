#ifndef OTTERENGINE_HASHUTILS_H
#define OTTERENGINE_HASHUTILS_H

#include "Core/BaseTypes.h"

namespace Otter::Internal
{
    /**
     * @brief A class that contains utilities for hashing.
     */
    class HashUtils final
    {
    public:
        /**
         * @brief Used to retrieve the next prime number after the given value.
         *
         * @param value The value to get the next prime number after.
         *
         * @return The next prime number after the given value.
         */
        [[nodiscard]] static UInt64 GetNextPrime(UInt64 value);

        /**
         * @brief Used to retrieve the previous prime number before the given value.
         *
         * @param value The value to get the previous prime number before.
         *
         * @return The previous prime number before the given value.
         */
        [[nodiscard]] static UInt64 GetPreviousPrime(UInt64 value);

    private:
        /**
         * @brief Constructor.
         */
        HashUtils() = default;

        /**
         * @brief Destructor.
         */
        ~HashUtils() = default;
    };
}

#endif //OTTERENGINE_HASHUTILS_H