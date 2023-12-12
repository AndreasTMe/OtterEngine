#ifndef OTTERENGINE_READONLYSPAN_H
#define OTTERENGINE_READONLYSPAN_H

#include "Core/Assert.h"
#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T, UInt64 Size>
    struct Span;

    /**
     * @brief A read-only span of a fixed size. All elements are stack allocated.
     *
     * @tparam T The type of the elements in the span.
     * @tparam Size The size of the span.
     */
    template<typename T, UInt64 Size>
    struct ReadOnlySpan final
    {
        /// @brief Alias for a const iterator.
        using ConstIterator = LinearIterator<const T>;

    public:
        /**
         * @brief Constructor.
         */
        ReadOnlySpan()
        {
            if (std::is_default_constructible<T>::value)
                for (UInt64 i = 0; i < Size; i++)
                    m_Data[i] = T();
        }

        /**
         * @brief Destructor.
         */
        ~ReadOnlySpan()
        {
            if (std::is_destructible<T>::value)
                for (UInt64 i = 0; i < Size; i++)
                    m_Data[i].~T();
        }

        /**
         * @brief Deleted copy constructor.
         */
        ReadOnlySpan(ReadOnlySpan&) = delete;

        /**
         * @brief Deleted copy constructor.
         */
        ReadOnlySpan(const ReadOnlySpan&) = delete;

        /**
         * @brief Deleted copy assignment operator.
         */
        ReadOnlySpan& operator=(const ReadOnlySpan&) = delete;

        /**
         * @brief Deleted move constructor.
         */
        ReadOnlySpan(ReadOnlySpan&&) = delete;

        /**
         * @brief Deleted move assignment operator.
         */
        ReadOnlySpan& operator=(ReadOnlySpan&&) = delete;

        /**
         * @brief Creates a read-only span from an initialiser list.
         *
         * @param list The initialiser list.
         */
        ReadOnlySpan(InitialiserList<T> list)
        {
            OTR_ASSERT_MSG(list.size() == Size, "Initialiser list size does not match span size")

            UInt64 i = 0;
            for (const T& value: list)
                m_Data[i++] = value;
        }

        /**
         * @brief Creates a read-only span by copying a normal span.
         *
         * @param other The span.
         */
        explicit ReadOnlySpan(const Span<T, Size>& other)
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];
        }

        /**
         * @brief Creates a read-only span by moving a normal span.
         *
         * @param other The span.
         */
        explicit ReadOnlySpan(Span<T, Size>&& other) noexcept
        {
            for (UInt64 i = 0; i < Size; i++)
            {
                m_Data[i]       = std::move(other.m_Data[i]);
                other.m_Data[i] = T();
            }
        }

        /**
         * @brief Gets the element at the specified index.
         *
         * @param index The index.
         *
         * @return The element at the specified index.
         */
        [[nodiscard]] OTR_INLINE const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(index < Size, "ReadOnlySpan index out of bounds")
            return m_Data[index];
        }

        /**
         * @brief Gets a pointer to the data in the span.
         *
         * @return A pointer to the data in the span.
         */
        [[nodiscard]] OTR_INLINE const T* GetData() const { return m_Data; }

        /**
         * @brief Gets the size of the span.
         *
         * @return The size of the span.
         */
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetSize() const { return Size; }

        /**
         * @brief Gets a const iterator to the first element in the span.
         *
         * @return A const iterator to the first element in the span.
         */
        OTR_INLINE ConstIterator begin() const noexcept { return ConstIterator(m_Data); }

        /**
         * @brief Gets a const iterator to the last element in the span.
         *
         * @return A const iterator to the last element in the span.
         */
        OTR_INLINE ConstIterator end() const noexcept { return ConstIterator(m_Data + Size); }

        /**
         * @brief Gets a reverse const iterator to the last element in the span.
         *
         * @return A reverse const iterator to the last element in the span.
         */
        OTR_INLINE ConstIterator rbegin() const noexcept { return ConstIterator(m_Data + Size - 1); }

        /**
         * @brief Gets a reverse const iterator to the first element in the span.
         *
         * @return A reverse const iterator to the first element in the span.
         */
        OTR_INLINE ConstIterator rend() const noexcept { return ConstIterator(m_Data - 1); }

    private:
        T m_Data[Size];
    };
}

#endif //OTTERENGINE_READONLYSPAN_H
