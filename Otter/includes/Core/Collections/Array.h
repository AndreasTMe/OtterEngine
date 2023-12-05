#ifndef OTTERENGINE_ARRAY_H
#define OTTERENGINE_ARRAY_H

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
    struct ReadOnlyArray;

    template<typename T, UInt64 Size>
    struct Array final
    {
    public:
        using Iterator = LinearIterator<T>;
        using ConstIterator = LinearIterator<const T>;

        Array()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, Size);

            if constexpr (Size > 0)
                m_Data = Buffer::New<T>(Size);
        }
        ~Array()
        {
            ClearDestructive();
        }

        OTR_WITH_ITERATOR(Iterator, m_Data, Size)
        OTR_WITH_CONST_ITERATOR(ConstIterator, m_Data, Size)

        Array(InitialiserList<T> list)
            : Array()
        {
            OTR_ASSERT_MSG(list.size() == Size, "Initialiser list size does not match span size")

            UInt64 i = 0;
            for (const T& value: list)
                m_Data[i++] = value;
        }

        Array(const Array<T, Size>& other)
            : Array()
        {
            m_Data = other.m_Data;
        }

        Array(Array<T, Size>&& other) noexcept
            : Array()
        {
            m_Data = std::move(other.m_Data);

            other.m_Data = nullptr;
        }

        Array<T, Size>& operator=(const Array<T, Size>& other)
        {
            if (this == &other)
                return *this;

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];

            return *this;
        }

        Array<T, Size>& operator=(Array<T, Size>&& other) noexcept
        {
            if (this == &other)
                return *this;

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);

            other.m_Data = nullptr;

            return *this;
        }

        T& operator[](UInt64 index)
        {
            OTR_ASSERT_MSG(IsCreated(), "Array has either not been created or has been destroyed")
            OTR_ASSERT_MSG(index < Size, "Array index out of bounds")
            return m_Data[index];
        }

        const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(IsCreated(), "Array has either not been created or has been destroyed")
            OTR_ASSERT_MSG(index < Size, "Array index out of bounds")
            return m_Data[index];
        }

        void ClearDestructive()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, Size);

            m_Data = nullptr;
        }

        [[nodiscard]] OTR_INLINE const ReadOnlyArray<T, Size> AsReadOnly() const
        {
            return ReadOnlyArray<T, Size>(*this);
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
        T* m_Data = nullptr;

        friend class ReadOnlyArray<T, Size>;
    };
}

#endif //OTTERENGINE_ARRAY_H
