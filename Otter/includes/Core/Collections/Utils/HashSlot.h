#ifndef OTTERENGINE_HASHSLOT_H
#define OTTERENGINE_HASHSLOT_H

#include "Core/BaseTypes.h"

namespace Otter
{
    /**
     * @brief An object that represents an item stored in a hash set or dictionary.
     *
     * @tparam T The type of the item.
     */
    template<typename T>
    struct Slot final
    {
    public:
        T      Data;
        UInt64 Hash;
        Slot* Next;

        /**
         * @brief Constructor.
         */
        Slot() : Data(), Hash(0), Next(nullptr) { }

        /**
         * @brief Destructor.
         */
        ~Slot()
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
                Data.~T();

            Hash = 0;
            Next = nullptr;
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The slot to copy.
         */
        Slot(const Slot<T>& other)
        {
            Data = other.Data;
            Hash = other.Hash;
            Next = other.Next;
        }

        /**
         * @brief Move constructor.
         *
         * @param other The slot to move.
         */
        Slot(Slot<T>&& other) noexcept
        {
            Data = std::move(other.Data);
            Hash = std::move(other.Hash);
            Next = std::move(other.Next);

            other.Next = nullptr;
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other The slot to copy.
         *
         * @return A reference to this slot.
         */
        Slot<T>& operator=(const Slot<T>& other)
        {
            if (this == &other)
                return *this;

            Data = other.Data;
            Hash = other.Hash;
            Next = other.Next;

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other The slot to move.
         *
         * @return A reference to this slot.
         */
        Slot<T>& operator=(Slot<T>&& other) noexcept
        {
            if (this == &other)
                return *this;

            Data = std::move(other.Data);
            Hash = std::move(other.Hash);
            Next = std::move(other.Next);

            other.Next = nullptr;

            return *this;
        }

        /**
         * @brief Sets the data of the slot.
         *
         * @param data The data to set.
         * @param hash The hash of the data.
         * @param next The next slot in the chain, if any.
         */
        OTR_INLINE void Set(const T& data, const UInt64 hash, Slot<T>* next = nullptr)
        {
            Data = data;
            Hash = hash;
            Next = next;
        }

        /**
         * @brief Checks if the slot matches the given data and hash.
         *
         * @param data The data to check.
         * @param hash The hash to check.
         *
         * @return True if the slot matches the data and hash, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool Matches(const T& data, const UInt64 hash) const
        {
            return Data == data && Hash == hash;
        }

        /**
         * @brief Checks if the key-value pair slot matches the given key and hash.
         *
         * @param key The key to check.
         * @param hash The hash to check.
         *
         * @return True if the key-value pair slot matches the key and hash, false otherwise.
         */
        template<typename TKey, typename TValue>
        requires (requires(T t) {
            { t.Key } -> std::convertible_to<TKey>;
            { t.Value } -> std::convertible_to<TValue>;
        })
        [[nodiscard]] OTR_INLINE bool MatchesKey(const TKey& key, const UInt64 hash) const
        {
            return Data.Key == key && Hash == hash;
        }
    };
}

#endif //OTTERENGINE_HASHSLOT_H
