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
        void Set(const T& data, const UInt64 hash, Slot<T>* next = nullptr)
        {
            Data = data;
            Hash = hash;
            Next = next;
        }
    };
}

#endif //OTTERENGINE_HASHSLOT_H
