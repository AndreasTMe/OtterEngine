#ifndef OTTERENGINE_BITSET_H
#define OTTERENGINE_BITSET_H

#include "Core/Memory.h"
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"

namespace Otter
{
#define UINT64_BITS 64

    class BitSet
    {
    public:
        BitSet()
            : m_Data(nullptr), m_Size(0)
        {
        }

        ~BitSet()
        {
            ClearDestructive();
        }

        BitSet(InitialiserList<bool> list)
            : BitSet()
        {
            RecreateEmpty(list.size() * 8);

            UInt64 index = 0;
            for (auto& item: list)
                Set(index++, item);
        }

        BitSet(const BitSet& other)
        {
            m_Size = other.m_Size;
            m_Data = Buffer::New<UInt64>(m_Size);

            for (UInt64 i = 0; i < m_Size; i++)
                m_Data[i] = other.m_Data[i];
        }

        BitSet(BitSet&& other) noexcept
        {
            m_Data = other.m_Data;
            m_Size = other.m_Size;

            other.m_Data = nullptr;
            other.m_Size = 0;
        }

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

        [[nodiscard]] OTR_INLINE bool operator==(const BitSet& other) const
        {
            if (m_Size != other.m_Size)
                return false;

            for (UInt64 i = 0; i < m_Size; i++)
                if (m_Data[i] != other.m_Data[i])
                    return false;

            return true;
        }

        [[nodiscard]] OTR_INLINE bool operator!=(const BitSet& other) const { return !(*this == other); }

        [[nodiscard]] bool Get(const UInt64 index) const
        {
            OTR_INTERNAL_ASSERT_MSG(index < m_Size * UINT64_BITS, "Index out of range")

            UInt64 i = index / UINT64_BITS;
            UInt64 j = index % UINT64_BITS;

            return (m_Data[i] & (1ULL << j)) != 0;
        }

        void Set(const UInt64 index, const bool value)
        {
            OTR_INTERNAL_ASSERT_MSG(index < m_Size * UINT64_BITS, "Index out of range")

            UInt64 i = index / UINT64_BITS;
            UInt64 j = index % UINT64_BITS;

            if (value)
                m_Data[i] |= (1ULL << j);
            else
                m_Data[i] &= ~(1ULL << j);
        }

        void Reserve(const UInt64 bitsSize)
        {
            RecreateEmpty(bitsSize);
        }

        void Expand(const UInt64 amount = 0)
        {
            UInt64 newSize = CalculateExpandSize(amount);

            auto* newData = Buffer::New<UInt64>(newSize);

            for (UInt64 i = 0; i < m_Size; i++)
                newData[i] = m_Data[i];

            if (IsCreated())
                Buffer::Delete<UInt64>(m_Data, m_Size);

            m_Data = newData;
            m_Size = newSize;
        }

        void Shrink(const UInt64 amount = 0)
        {
            UInt64 newSize = CalculateShrinkSize(amount);

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

        void Clear()
        {
            for (UInt64 i = 0; i < m_Size; i++)
                m_Data[i] = 0;
        }

        void ClearDestructive()
        {
            if (IsCreated())
                Buffer::Delete<UInt64>(m_Data, m_Size);

            m_Size = 0;
            m_Data = nullptr;
        }

        [[nodiscard]] OTR_INLINE UInt64 GetSize() const { return m_Size; }

        [[nodiscard]] OTR_INLINE UInt64 GetBitSize() const { return m_Size * sizeof(UInt64) * 8; }

        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept { return m_Data && m_Size > 0; }

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

    private:
        UInt64* m_Data;
        UInt64 m_Size;

        void RecreateEmpty(const UInt64 bitsSize)
        {
            if (IsCreated())
                Buffer::Delete<UInt64>(m_Data, m_Size);

            m_Size = GetActualOrMinimumSize(bitsSize);
            m_Data = Buffer::New<UInt64>(m_Size);
        }

        [[nodiscard]] UInt64 CalculateExpandSize(const UInt64 expandAmount) const
        {
            const UInt64 itemsSize = m_Size * UINT64_BITS;
            UInt64       newSize;

            if (expandAmount == 0)
                newSize = itemsSize == 0 ? 0 : itemsSize * 1.5; // NOLINT(*-narrowing-conversions)
            else
                newSize = itemsSize + expandAmount;

            return GetActualOrMinimumSize(newSize);
        }

        [[nodiscard]] UInt64 CalculateShrinkSize(const UInt64 shrinkAmount) const
        {
            if (m_Size == 0)
                return 0;

            const UInt64 itemsSize = m_Size * UINT64_BITS;
            UInt64       newSize;

            if (shrinkAmount == 0)
                newSize = itemsSize * 0.75; // NOLINT(*-narrowing-conversions)
            else if (shrinkAmount > itemsSize)
                newSize = 0;
            else
                newSize = itemsSize - shrinkAmount;

            return GetActualOrMinimumSize(newSize);
        }

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
