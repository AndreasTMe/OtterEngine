#ifndef OTTERENGINE_SPAN_H
#define OTTERENGINE_SPAN_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"

#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T, UInt64 Size>
    class ReadOnlySpan;

    template<typename T, UInt64 Size>
    class Span
    {
    public:
        using Iterator = LinearIterator<T>;
        using ConstIterator = LinearIterator<const T>;

        OTR_WITH_DEFAULT_CONSTRUCTOR(Span)
        OTR_WITH_ITERATOR(Iterator, m_Data, Size)
        OTR_WITH_CONST_ITERATOR(ConstIterator, m_Data, Size)
        OTR_DISABLE_HEAP_ALLOCATION

        OTR_INLINE explicit Span(const T& value)
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = value;
        }

        template<typename... Args>
        OTR_INLINE explicit Span(const T& value, const Args&& ... args)
            : m_Data{ value, args... }
        {
            OTR_ASSERT_MSG(sizeof...(Args) < Size, "Span size is too small")
        }

        OTR_INLINE Span(const Span<T, Size>& other)
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];
        }

        OTR_INLINE Span(Span<T, Size>&& other) noexcept
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);
        }

        OTR_INLINE Span<T, Size>& operator=(const Span<T, Size>& other)
        {
            if (this == &other)
                return *this;

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];

            return *this;
        }

        OTR_INLINE Span<T, Size>& operator=(Span<T, Size>&& other) noexcept
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);

            return *this;
        }

        OTR_INLINE T& operator[](UInt64 index)
        {
            OTR_ASSERT_MSG(index < Size, "Span index out of bounds")
            return m_Data[index];
        }

        OTR_INLINE const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(index < Size, "Span index out of bounds")
            return m_Data[index];
        }

        [[nodiscard]] OTR_INLINE T* GetData() { return m_Data; }
        [[nodiscard]] OTR_INLINE const T* GetData() const { return m_Data; }

        [[nodiscard]] OTR_INLINE const ReadOnlySpan<T, Size> AsReadOnly() const
        {
            return ReadOnlySpan<T, Size>(*this);
        }

        [[nodiscard]] OTR_INLINE constexpr UInt64 Length() const { return Size; }

    private:
        T m_Data[Size];

        friend class ReadOnlySpan<T, Size>;
    };
}

template<typename OStream, typename T, UInt64 Size>
OTR_INLINE OStream& operator<<(OStream& os, const Otter::Span<T, Size>& array)
{
    os << "Span: [";

    for (UInt64 i = 0; i < Size; i++)
    {
        os << array[i];

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

#endif //OTTERENGINE_SPAN_H
