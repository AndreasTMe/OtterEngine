#ifndef OTTERENGINE_READONLYSPAN_H
#define OTTERENGINE_READONLYSPAN_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"

#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T, UInt64 Size>
    struct Span;

    template<typename T, UInt64 Size>
    struct ReadOnlySpan final
    {
    public:
        using ConstIterator = LinearIterator<const T>;

        ReadOnlySpan()
        {
            if (std::is_default_constructible<T>::value)
                for (UInt64 i = 0; i < Size; i++)
                    m_Data[i] = T();
        }
        ~ReadOnlySpan()
        {
            if (std::is_destructible<T>::value)
                for (UInt64 i = 0; i < Size; i++)
                    m_Data[i].~T();
        }

        OTR_WITH_CONST_ITERATOR(ConstIterator, m_Data, Size)
        OTR_DISABLE_OBJECT_COPIES(ReadOnlySpan)
        OTR_DISABLE_OBJECT_MOVES(ReadOnlySpan)
        OTR_DISABLE_HEAP_ALLOCATION

        OTR_INLINE explicit ReadOnlySpan(const T& value)
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = value;
        }

        template<typename... Args>
        OTR_INLINE explicit ReadOnlySpan(const T& value, const Args&& ... args)
            : m_Data{ value, args... }
        {
            OTR_ASSERT_MSG(sizeof...(Args) < Size, "ReadOnlySpan size is too small")
        }

        OTR_INLINE explicit ReadOnlySpan(const Span<T, Size>& other)
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];
        }

        OTR_INLINE explicit ReadOnlySpan(Span<T, Size>&& other) noexcept
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);
        }

        OTR_INLINE ReadOnlySpan<T, Size>& operator=(const Span<T, Size>& other)
        {
            if (this == &other)
                return *this;

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];

            return *this;
        }

        OTR_INLINE ReadOnlySpan<T, Size>& operator=(Span<T, Size>&& other) noexcept
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);

            return *this;
        }

        OTR_INLINE const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(index < Size, "ReadOnlySpan index out of bounds")
            return m_Data[index];
        }

    private:
        T m_Data[Size];
    };
}

template<typename OStream, typename T, UInt64 Size>
OTR_INLINE OStream& operator<<(OStream& os, const Otter::ReadOnlySpan<T, Size>& span)
{
    os << "ReadOnlySpan: [";

    for (UInt64 i = 0; i < Size; i++)
    {
        os << span[i];

        if (i >= 2)
        {
            os << ", ...";
            break;
        }

        if (i != Size - 1)
            os << ", ";
    }

    os << "]";

    return os;
}

#endif //OTTERENGINE_READONLYSPAN_H
