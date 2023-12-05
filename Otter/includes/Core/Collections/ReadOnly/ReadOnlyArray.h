#ifndef OTTERENGINE_READONLYARRAY_H
#define OTTERENGINE_READONLYARRAY_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Core/Memory.h"

#include "Core/Collections/Iterators/LinearIterator.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    template<typename T, UInt64 Size>
    struct Array;

    template<typename T, UInt64 Size>
    struct ReadOnlyArray final
    {
    public:
        using ConstIterator = LinearIterator<const T>;

        ReadOnlyArray()
        {
            if constexpr (Size > 0)
                m_Data = Buffer::New<T>(Size);
        }
        ~ReadOnlyArray()
        {
            ClearDestructive();
        }

        OTR_WITH_CONST_ITERATOR(ConstIterator, m_Data, Size)
        OTR_DISABLE_OBJECT_COPIES(ReadOnlyArray)
        OTR_DISABLE_OBJECT_MOVES(ReadOnlyArray)

        ReadOnlyArray(InitialiserList<T> list)
            : ReadOnlyArray()
        {
            OTR_ASSERT_MSG(list.size() == Size, "Initialiser list size does not match span size")

            UInt64 i = 0;
            for (const T& value: list)
                m_Data[i++] = value;
        }

        explicit ReadOnlyArray(const Array<T, Size>& other)
            : ReadOnlyArray()
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];
        }

        explicit ReadOnlyArray(Array<T, Size>&& other) noexcept
            : ReadOnlyArray()
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);

            other.m_Data = nullptr;
        }

        const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(IsCreated(), "Array has either not been created or has been destroyed")
            OTR_ASSERT_MSG(index < Size, "ReadOnlyArray index out of bounds")
            return m_Data[index];
        }

        void ClearDestructive()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, Size);

            m_Data = nullptr;
        }

#if !OTR_RUNTIME
        ReadOnlySpan<MemoryFootprint, 1> GetMemoryFootprint(const char* const debugName) const
        {
            MemoryFootprint footprint = { };
            OTR_MEMORY_SYSTEM.CheckMemoryFootprint([&]()
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

        [[nodiscard]] OTR_INLINE const T* GetData() const { return m_Data; }
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetSize() const { return Size; }
        [[nodiscard]] OTR_INLINE constexpr bool IsCreated() const { return m_Data && Size > 0; }

    private:
        T* m_Data;
    };
}

#endif //OTTERENGINE_READONLYARRAY_H
