#ifndef OTTERENGINE_BITSET_H
#define OTTERENGINE_BITSET_H

#include "Core/Memory.h"
#include "Core/Collections/Iterators/LinearIterator.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
#define UINT64_BITS 64

    /**
     * @brief A bitset of a fixed size. All elements are heap allocated.
     *
     * @note Under the hood, the bitset uses an array of 64-bit unsigned integers.
     */
    class BitSet
    {
    public:
        /**
         * @brief Constructor.
         */
        BitSet() = default;

        /**
         * @brief Destructor.
         */
        ~BitSet()
        {
            if (IsCreated())
                Buffer::Delete<UInt64>(m_Data, m_Size);

            m_Data = nullptr;
        }

        /**
         * @brief Creates a bitset from an initialiser list.
         *
         * @param list The initialiser list.
         */
        BitSet(InitialiserList<bool> list)
            : BitSet()
        {
            RecreateEmpty(list.size() * 8);

            UInt64 index = 0;
            for (auto& item: list)
                Set(index++, item);
        }

        /**
         * @brief Copy constructor.
         *
         * @param other BitSet to copy from.
         */
        BitSet(const BitSet& other)
        {
            m_Size = other.m_Size;
            m_Data = Buffer::New<UInt64>(m_Size);

            for (UInt64 i = 0; i < m_Size; i++)
                m_Data[i] = other.m_Data[i];
        }

        /**
         * @brief Move constructor.
         *
         * @param other BitSet to move from.
         */
        BitSet(BitSet&& other) noexcept
        {
            m_Data = other.m_Data;
            m_Size = other.m_Size;

            other.m_Data = nullptr;
            other.m_Size = 0;
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other BitSet to copy from.
         *
         * @return Reference to this bitset.
         */
        BitSet& operator=(const BitSet& other)
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<UInt64>(m_Data, m_Size);

            m_Size = other.m_Size;
            m_Data = Buffer::New<UInt64>(m_Size);

            for (UInt64 i = 0; i < m_Size; i++)
                m_Data[i] = other.m_Data[i];

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other BitSet to move from.
         *
         * @return Reference to this bitset.
         */
        BitSet& operator=(BitSet&& other) noexcept
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<UInt64>(m_Data, m_Size);

            m_Data = other.m_Data;
            m_Size = other.m_Size;

            other.m_Data = nullptr;
            other.m_Size = 0;

            return *this;
        }

        /**
         * @brief Equality operator.
         *
         * @param other BitSet to compare to.
         *
         * @return True if the bitsets are equal, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool operator==(const BitSet& other) const
        {
            if (m_Size != other.m_Size)
                return false;

            for (UInt64 i = 0; i < m_Size; i++)
                if (m_Data[i] != other.m_Data[i])
                    return false;

            return true;
        }

        /**
         * @brief Inequality operator.
         *
         * @param other BitSet to compare to.
         *
         * @return True if the bitsets are not equal, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool operator!=(const BitSet& other) const { return !(*this == other); }

        /**
         * @brief Gets the bit at the specified index.
         *
         * @param index The index.
         *
         * @return The bit at the specified index.
         */
        [[nodiscard]] bool Get(const UInt64 index) const
        {
            if (!IsCreated())
                return false;

            OTR_INTERNAL_ASSERT_MSG(index < GetBitsSize(), "Index out of range")

            UInt64 i = index / UINT64_BITS;
            UInt64 j = index % UINT64_BITS;

            return (m_Data[i] & (1ULL << j)) != 0;
        }

        /**
         * @brief Sets the bit at the specified index.
         *
         * @param index The index.
         * @param value The value to set.
         */
        void Set(const UInt64 index, const bool value)
        {
            if (!IsCreated())
                Reserve(index + 1);

            if (index >= GetBitsSize())
                Expand(index - GetBitsSize() + 1);

            UInt64 i = index / UINT64_BITS;
            UInt64 j = index % UINT64_BITS;

            if (value)
                m_Data[i] |= (1ULL << j);
            else
                m_Data[i] &= ~(1ULL << j);
        }

        /**
         * @brief Used to reserve memory for the bitset.
         *
         * @param bitsSize The size of the bitset in bits.
         *
         * @note This operation is destructive and will delete any existing data.
         */
        void Reserve(const UInt64 bitsSize)
        {
            RecreateEmpty(bitsSize);
        }

        /**
         * @brief Used to expand the size of the bitset by a given amount.
         *
         * @param bitAmount The amount to expand the bitset by.
         */
        void Expand(const UInt64 bitAmount = 0)
        {
            UInt64 newSize = CalculateExpandSize(bitAmount);

            auto* newData = Buffer::New<UInt64>(newSize);

            for (UInt64 i = 0; i < m_Size; i++)
                newData[i] = m_Data[i];

            if (IsCreated())
                Buffer::Delete<UInt64>(m_Data, m_Size);

            m_Data = newData;
            m_Size = newSize;
        }

        /**
         * @brief Used to shrink the size of the bitset by a given amount.
         *
         * @param bitAmount The amount to shrink the bitset by.
         */
        void Shrink(const UInt64 bitAmount = 0)
        {
            UInt64 newSize = CalculateShrinkSize(bitAmount);

            if (IsEmpty() || newSize == 0)
            {
                RecreateEmpty(newSize);
                return;
            }

            auto* newData = Buffer::New<UInt64>(newSize);

            for (UInt64 i = 0; i < newSize; i++)
                newData[i] = m_Data[i];

            if (IsCreated())
                Buffer::Delete<UInt64>(m_Data, m_Size);

            m_Data = newData;
            m_Size = newSize;
        }

        /**
         * @brief Clears the bitset.
         */
        void Clear()
        {
            for (UInt64 i = 0; i < m_Size; i++)
                m_Data[i] = 0;
        }

        /**
         * @brief Clears the bitset and deletes the data.
         */
        void ClearDestructive()
        {
            if (IsCreated())
                Buffer::Delete<UInt64>(m_Data, m_Size);

            m_Size = 0;
            m_Data = nullptr;
        }

#if !OTR_RUNTIME
        /**
         * @brief Gets the memory footprint of the bitset.
         *
         * @param debugName The name used for debugging.
         *
         * @return The memory footprint of the bitset.
         */
        ReadOnlySpan<MemoryFootprint, 1> GetMemoryFootprint(const char* const debugName) const
        {
            MemoryFootprint footprint = { };
            MemorySystem::CheckMemoryFootprint([&]()
                                               {
                                                   MemoryDebugPair pair[1];
                                                   pair[0] = { debugName, m_Data };

                                                   return MemoryDebugHandle{ pair, 1 };
                                               },
                                               &footprint,
                                               nullptr);

            return ReadOnlySpan<MemoryFootprint, 1>{ footprint };
        }
#endif

        /**
         * @brief Gets the size of the bitset.
         *
         * @return The size of the bitset.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetSize() const { return m_Size; }

        /**
         * @brief Gets the size of the bitset in bits.
         *
         * @return The size of the bitset in bits.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetBitsSize() const { return m_Size * UINT64_BITS; }

        /**
         * @brief Checks whether the bitset has been created. A bitset is created when it has been initialised
         * with a valid size and has not been destroyed.
         *
         * @return True if the bitset has been created, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept { return m_Data && m_Size > 0; }

        /**
         * @brief Checks whether the bitset is empty.
         *
         * @return True if the bitset is empty, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsEmpty() const { return m_Size == 0; }

        /**
         * @brief Gets the hash code of the bitset.
         *
         * @return Hash code of the bitset.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetHashCode() const noexcept
        {
            UInt64 hash = 0;

            for (UInt64 i = 0; i < m_Size * UINT64_BITS; i++)
                hash ^= std::hash<bool>{ }(m_Data[i]) << i;

            return hash;
        }

    private:
        UInt64* m_Data = nullptr;
        UInt64 m_Size = 0;

        /**
         * @brief Recreates the bitset with a given size. Deletes any existing data.
         *
         * @param bitsSize The size to recreate the bitset with in bits.
         */
        void RecreateEmpty(const UInt64 bitsSize)
        {
            if (IsCreated())
                Buffer::Delete<UInt64>(m_Data, m_Size);

            m_Size = GetActualOrMinimumSize(bitsSize);
            m_Data = Buffer::New<UInt64>(m_Size);
        }

        /**
         * @brief Calculates the new size when expanding the bitset.
         *
         * @param expandBitAmount The amount to expand the bitset by in bits.
         *
         * @return The new size.
         */
        [[nodiscard]] UInt64 CalculateExpandSize(const UInt64 expandBitAmount) const
        {
            const UInt64 itemsSize = m_Size * UINT64_BITS;
            UInt64       newSize;

            if (expandBitAmount == 0)
                newSize = itemsSize == 0 ? 0 : itemsSize * 1.5; // NOLINT(*-narrowing-conversions)
            else
                newSize = itemsSize + expandBitAmount;

            return GetActualOrMinimumSize(newSize);
        }

        /**
         * @brief Calculates the new size when shrinking the bitset.
         *
         * @param shrinkBitAmount The amount to shrink the bitset by in bits.
         *
         * @return The new size.
         */
        [[nodiscard]] UInt64 CalculateShrinkSize(const UInt64 shrinkBitAmount) const
        {
            if (m_Size == 0)
                return 0;

            const UInt64 itemsSize = m_Size * UINT64_BITS;
            UInt64       newSize;

            if (shrinkBitAmount == 0)
                newSize = itemsSize * 0.75; // NOLINT(*-narrowing-conversions)
            else if (shrinkBitAmount > itemsSize)
                newSize = 0;
            else
                newSize = itemsSize - shrinkBitAmount;

            return GetActualOrMinimumSize(newSize);
        }

        /**
         * @brief Gets the actual size or the minimum size.
         *
         * @param bitsSize The size in bits.
         *
         * @return The actual size or the minimum size (not in bits).
         */
        [[nodiscard]] OTR_INLINE static UInt64 GetActualOrMinimumSize(const UInt64 bitsSize)
        {
            if (bitsSize < UINT64_BITS * 2)
                return 2;

            return bitsSize % UINT64_BITS == 0
                   ? bitsSize / UINT64_BITS
                   : bitsSize / UINT64_BITS + 1;
        }
    };

#undef UINT64_BITS
}

#endif //OTTERENGINE_BITSET_H
