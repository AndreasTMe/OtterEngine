#ifndef OTTERENGINE_SPAN_H
#define OTTERENGINE_SPAN_H

#include "Core/Assert.h"
#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T, UInt64 Size>
    class ReadOnlySpan;

    /**
     * @brief A span of a fixed size. All elements are stack allocated.
     *
     * @tparam T The type of the elements in the span.
     * @tparam Size The size of the span.
     */
    template<typename T, UInt64 Size>
    class Span final
    {
        /// @brief Alias for an iterator.
        using Iterator = LinearIterator<T>;

        /// @brief Alias for a const iterator.
        using ConstIterator = LinearIterator<const T>;

    public:
        /**
         * @brief Constructor.
         */
        Span()
        {
            if (std::is_default_constructible<T>::value)
                for (UInt64 i = 0; i < Size; i++)
                    m_Data[i] = T();
        }

        /**
         * @brief Destructor.
         */
        ~Span()
        {
            if (std::is_destructible<T>::value)
                for (UInt64 i = 0; i < Size; i++)
                    m_Data[i].~T();
        }

        /**
         * @brief Creates a span from an initialiser list.
         *
         * @param list The initialiser list.
         */
        Span(InitialiserList<T> list)
        {
            OTR_ASSERT(list.size() == Size, "Initialiser list size does not match span size")

            UInt64 i = 0;
            for (const T& value: list)
                m_Data[i++] = value;
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The other span to copy.
         */
        Span(const Span<T, Size>& other)
        {
            if constexpr (Size == 0)
                return;
            else
            {
                for (UInt64 i = 0; i < Size; i++)
                    m_Data[i] = other.m_Data[i];
            }
        }

        /**
         * @brief Move constructor.
         *
         * @param other The other span to move.
         */
        Span(Span<T, Size>&& other) noexcept
        {
            for (UInt64 i = 0; i < Size; i++)
            {
                m_Data[i] = std::move(other.m_Data[i]);

                if constexpr (std::is_default_constructible<T>::value)
                    other.m_Data[i] = T();
            }
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other The other span to copy.
         *
         * @return A reference to this span.
         */
        Span<T, Size>& operator=(const Span<T, Size>& other)
        {
            if (this == &other)
                return *this;

            if constexpr (Size == 0)
                m_Data = nullptr;
            else
            {
                for (UInt64 i = 0; i < Size; i++)
                    m_Data[i] = other.m_Data[i];
            }

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other The other span to move.
         *
         * @return A reference to this span.
         */
        Span<T, Size>& operator=(Span<T, Size>&& other) noexcept
        {
            if (this == &other)
                return *this;

            for (UInt64 i = 0; i < Size; i++)
            {
                m_Data[i] = std::move(other.m_Data[i]);

                if constexpr (std::is_default_constructible<T>::value)
                    other.m_Data[i] = T();
            }

            return *this;
        }

        /**
         * @brief Equality operator.
         *
         * @param other The span to compare to.
         *
         * @return True if the spans are equal, false otherwise.
         */
        bool operator==(const Span<T, Size>& other) const
        {
            if constexpr (Size == 0)
                return true;
            else
            {
                for (UInt64 i = 0; i < Size; ++i)
                    if (m_Data[i] != other.m_Data[i])
                        return false;

                return true;
            }
        }

        /**
         * @brief Inequality operator.
         *
         * @param other The span to compare to.
         *
         * @return True if the spans are not equal, false otherwise.
         */
        bool operator!=(const Span<T, Size>& other) const { return !(*this == other); }

        /**
         * @brief Gets the element at the specified index.
         *
         * @param index The index.
         *
         * @return The element at the specified index.
         */
        [[nodiscard]] OTR_INLINE T& operator[](UInt64 index)
        {
            OTR_ASSERT(index < Size, "Span index out of bounds")
            return m_Data[index];
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
            OTR_ASSERT(index < Size, "Span index out of bounds")
            return m_Data[index];
        }

        /**
         * @brief Converts this span to a read-only span.
         *
         * @return The read-only span.
         */
        [[nodiscard]] OTR_INLINE const ReadOnlySpan<T, Size> AsReadOnly() const
        {
            return ReadOnlySpan<T, Size>(*this);
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
         * @brief Gets an iterator to the first element of the span.
         *
         * @return An iterator to the first element of the span.
         */
        OTR_INLINE Iterator begin() noexcept { return Iterator(m_Data); }

        /**
         * @brief Gets an iterator to the last element of the span.
         *
         * @return An iterator to the last element of the span.
         */
        OTR_INLINE Iterator end() noexcept { return Iterator(m_Data + Size); }

        /**
         * @brief Gets a reverse iterator to the last element of the span.
         *
         * @return A reverse iterator to the last element of the span.
         */
        OTR_INLINE Iterator rbegin() noexcept { return Iterator(m_Data + Size - 1); }

        /**
         * @brief Gets a reverse iterator to the first element of the span.
         *
         * @return A reverse iterator to the first element of the span.
         */
        OTR_INLINE Iterator rend() noexcept { return Iterator(m_Data - 1); }

        /**
         * @brief Gets a const iterator to the first element of the span.
         *
         * @return A const iterator to the first element of the span.
         */
        OTR_INLINE ConstIterator cbegin() const noexcept { return ConstIterator(m_Data); }

        /**
         * @brief Gets a const iterator to the last element of the span.
         *
         * @return A const iterator to the last element of the span.
         */
        OTR_INLINE ConstIterator cend() const noexcept { return ConstIterator(m_Data + Size); }

        /**
         * @brief Gets a reverse const iterator to the last element of the span.
         *
         * @return A reverse const iterator to the last element of the span.
         */
        OTR_INLINE ConstIterator crbegin() const noexcept { return ConstIterator(m_Data + Size - 1); }

        /**
         * @brief Gets a reverse const iterator to the first element of the span.
         *
         * @return A reverse const iterator to the first element of the span.
         */
        OTR_INLINE ConstIterator crend() const noexcept { return ConstIterator(m_Data - 1); }

    private:
        T m_Data[Size];

        friend class ReadOnlySpan<T, Size>;
    };
}

#endif //OTTERENGINE_SPAN_H
