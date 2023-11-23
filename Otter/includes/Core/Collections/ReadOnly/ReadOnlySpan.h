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

        ReadOnlySpan(InitialiserList<T> list)
        {
            OTR_ASSERT_MSG(list.size() == Size, "Initialiser list size does not match span size")

            UInt64 i = 0;
            for (const T& value: list)
                m_Data[i++] = value;
        }

        explicit ReadOnlySpan(const Span<T, Size>& other)
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];
        }

        explicit ReadOnlySpan(Span<T, Size>&& other) noexcept
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);
        }

        ReadOnlySpan<T, Size>& operator=(const Span<T, Size>& other)
        {
            if (this == &other)
                return *this;

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];

            return *this;
        }

        ReadOnlySpan<T, Size>& operator=(Span<T, Size>&& other) noexcept
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);

            return *this;
        }

        const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(index < Size, "ReadOnlySpan index out of bounds")
            return m_Data[index];
        }

        [[nodiscard]] OTR_INLINE const T* GetData() const { return m_Data; }
        [[nodiscard]] OTR_INLINE constexpr UInt64 Length() const { return Size; }

    private:
        T m_Data[Size];
    };
}

template<typename OStream, typename T, UInt64 Size>
OStream& operator<<(OStream& os, const Otter::ReadOnlySpan<T, Size>& span)
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
