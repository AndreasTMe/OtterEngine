#ifndef OTTERENGINE_SPAN_H
#define OTTERENGINE_SPAN_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"

#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T, UInt64 Size>
    struct ReadOnlySpan;

    template<typename T, UInt64 Size>
    struct Span final
    {
    public:
        using Iterator = LinearIterator<T>;
        using ConstIterator = LinearIterator<const T>;

        Span()
        {
            if (std::is_default_constructible<T>::value)
                for (UInt64 i = 0; i < Size; i++)
                    m_Data[i] = T();
        }
        ~Span()
        {
            if (std::is_destructible<T>::value)
                for (UInt64 i = 0; i < Size; i++)
                    m_Data[i].~T();
        }

        OTR_WITH_ITERATOR(Iterator, m_Data, Size)
        OTR_WITH_CONST_ITERATOR(ConstIterator, m_Data, Size)

        Span(InitialiserList<T> list)
        {
            OTR_ASSERT_MSG(list.size() == Size, "Initialiser list size does not match span size")

            UInt64 i = 0;
            for (const T& value: list)
                m_Data[i++] = value;
        }

        Span(const Span<T, Size>& other)
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];
        }

        Span(Span<T, Size>&& other) noexcept
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);
        }

        Span<T, Size>& operator=(const Span<T, Size>& other)
        {
            if (this == &other)
                return *this;

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];

            return *this;
        }

        Span<T, Size>& operator=(Span<T, Size>&& other) noexcept
        {
            if (this == &other)
                return *this;

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);

            return *this;
        }

        T& operator[](UInt64 index)
        {
            OTR_ASSERT_MSG(index < Size, "Span index out of bounds")
            return m_Data[index];
        }

        const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(index < Size, "Span index out of bounds")
            return m_Data[index];
        }

        [[nodiscard]] OTR_INLINE const ReadOnlySpan<T, Size> AsReadOnly() const
        {
            return ReadOnlySpan<T, Size>(*this);
        }

        [[nodiscard]] OTR_INLINE const T* GetData() const { return m_Data; }
        [[nodiscard]] OTR_INLINE constexpr UInt64 Length() const { return Size; }

    private:
        T m_Data[Size];

        friend class ReadOnlySpan<T, Size>;
    };
}

template<typename OStream, typename T, UInt64 Size>
OStream& operator<<(OStream& os, const Otter::Span<T, Size>& span)
{
    os << "Span: [";

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

#endif //OTTERENGINE_SPAN_H
