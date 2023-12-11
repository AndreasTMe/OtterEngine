#ifndef OTTERENGINE_SPAN_H
#define OTTERENGINE_SPAN_H

#include "Core/Assert.h"
#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T, UInt64 Size>
    struct ReadOnlySpan;

    template<typename T, UInt64 Size>
    struct Span final
    {
        using Iterator = LinearIterator<T>;
        using ConstIterator = LinearIterator<const T>;

    public:
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
            {
                m_Data[i]       = std::move(other.m_Data[i]);
                other.m_Data[i] = T();
            }
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
            {
                m_Data[i]       = std::move(other.m_Data[i]);
                other.m_Data[i] = T();
            }

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
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetSize() const { return Size; }

        OTR_INLINE Iterator begin() noexcept { return Iterator(m_Data); }
        OTR_INLINE Iterator end() noexcept { return Iterator(m_Data + Size); }
        OTR_INLINE Iterator rbegin() noexcept { return Iterator(m_Data + Size - 1); }
        OTR_INLINE Iterator rend() noexcept { return Iterator(m_Data - 1); }

        OTR_INLINE ConstIterator begin() const noexcept { return ConstIterator(m_Data); }
        OTR_INLINE ConstIterator end() const noexcept { return ConstIterator(m_Data + Size); }
        OTR_INLINE ConstIterator rbegin() const noexcept { return ConstIterator(m_Data + Size - 1); }
        OTR_INLINE ConstIterator rend() const noexcept { return ConstIterator(m_Data - 1); }

    private:
        T m_Data[Size];

        friend class ReadOnlySpan<T, Size>;
    };
}

#endif //OTTERENGINE_SPAN_H
