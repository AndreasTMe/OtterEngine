#ifndef OTTERENGINE_READONLYSPAN_H
#define OTTERENGINE_READONLYSPAN_H

#include "Core/Assert.h"
#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T, UInt64 Size>
    struct Span;

    template<typename T, UInt64 Size>
    struct ReadOnlySpan final
    {
        using ConstIterator = LinearIterator<const T>;

    public:
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

        ReadOnlySpan(ReadOnlySpan&) = delete;
        ReadOnlySpan(const ReadOnlySpan&) = delete;
        ReadOnlySpan& operator=(const ReadOnlySpan&) = delete;

        ReadOnlySpan(ReadOnlySpan&&) = delete;
        ReadOnlySpan& operator=(ReadOnlySpan&&) = delete;

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
            {
                m_Data[i]       = std::move(other.m_Data[i]);
                other.m_Data[i] = T();
            }
        }

        const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(index < Size, "ReadOnlySpan index out of bounds")
            return m_Data[index];
        }

        [[nodiscard]] OTR_INLINE const T* GetData() const { return m_Data; }
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetSize() const { return Size; }

        OTR_INLINE ConstIterator begin() const noexcept { return ConstIterator(m_Data); }
        OTR_INLINE ConstIterator end() const noexcept { return ConstIterator(m_Data + Size); }
        OTR_INLINE ConstIterator rbegin() const noexcept { return ConstIterator(m_Data + Size - 1); }
        OTR_INLINE ConstIterator rend() const noexcept { return ConstIterator(m_Data - 1); }

    private:
        T m_Data[Size];
    };
}

#endif //OTTERENGINE_READONLYSPAN_H
