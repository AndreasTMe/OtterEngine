#ifndef OTTERENGINE_ARRAY_H
#define OTTERENGINE_ARRAY_H

#include "Core/Memory.h"
#include "Core/Collections/Iterators/LinearIterator.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    template<typename T, UInt64 Size>
    struct ReadOnlyArray;

    /**
     * @brief An array of a fixed size. All elements are heap allocated.
     *
     * @tparam T The type of the elements in the array.
     * @tparam Size The size of the array.
     */
    template<typename T, UInt64 Size>
    struct Array final
    {
        /// @brief Alias for an iterator.
        using Iterator = LinearIterator<T>;

        /// @brief Alias for a const iterator.
        using ConstIterator = LinearIterator<const T>;

    public:
        /**
         * @brief Constructor.
         */
        Array()
        {
            if constexpr (Size > 0)
                m_Data = Buffer::New<T>(Size);
        }

        /**
         * @brief Destructor.
         */
        ~Array()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, Size);

            m_Data = nullptr;
        }

        /**
         * @brief Creates an array from an initialiser list.
         *
         * @param list The initialiser list.
         */
        Array(InitialiserList<T> list)
            : Array()
        {
            OTR_ASSERT_MSG(list.size() == Size, "Initialiser list size does not match span size")

            UInt64 i = 0;
            for (const T& value: list)
                m_Data[i++] = value;
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The other array to copy.
         */
        Array(const Array<T, Size>& other)
        {
            m_Data = Buffer::New<T>(Size);

            if constexpr (Size > 0)
                MemorySystem::MemoryCopy(m_Data, other.m_Data, Size * sizeof(T));
        }

        /**
         * @brief Move constructor.
         *
         * @param other The other array to move.
         */
        Array(Array<T, Size>&& other) noexcept
        {
            m_Data = std::move(other.m_Data);

            other.m_Data = nullptr;
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other The other array to copy.
         *
         * @return A reference to this array.
         */
        Array<T, Size>& operator=(const Array<T, Size>& other)
        {
            if (this == &other)
                return *this;

            m_Data = Buffer::New<T>(Size);

            if constexpr (Size > 0)
                MemorySystem::MemoryCopy(m_Data, other.m_Data, Size * sizeof(T));

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other The other array to move.
         *
         * @return A reference to this array.
         */
        Array<T, Size>& operator=(Array<T, Size>&& other) noexcept
        {
            if (this == &other)
                return *this;

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);

            other.m_Data = nullptr;

            return *this;
        }

        /**
         * @brief Equality operator.
         *
         * @param other The array to compare to.
         *
         * @return True if the arrays are equal, false otherwise.
         */
        [[nodiscard]] bool operator==(const Array<T, Size>& other) const { return m_Data == other.m_Data; }

        /**
         * @brief Inequality operator.
         *
         * @param other The array to compare to.
         *
         * @return True if the arrays are not equal, false otherwise.
         */
        [[nodiscard]] bool operator!=(const Array<T, Size>& other) const { return !(*this == other); }

        /**
         * @brief Gets the element at the specified index.
         *
         * @param index The index.
         *
         * @return The element at the specified index.
         */
        [[nodiscard]] OTR_INLINE T& operator[](UInt64 index)
        {
            OTR_ASSERT_MSG(IsCreated(), "Array has either not been created or has been destroyed")
            OTR_ASSERT_MSG(index < Size, "Array index out of bounds")
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
            OTR_ASSERT_MSG(IsCreated(), "Array has either not been created or has been destroyed")
            OTR_ASSERT_MSG(index < Size, "Array index out of bounds")
            return m_Data[index];
        }

        /**
         * @brief Converts the array to a read only array.
         *
         * @return The read only array.
         */
        [[nodiscard]] OTR_INLINE const ReadOnlyArray<T, Size> AsReadOnly() const
        {
            return ReadOnlyArray<T, Size>(*this);
        }

#if !OTR_RUNTIME
        /**
         * @brief Gets the memory footprint of the array.
         *
         * @param debugName The name used for debugging.
         *
         * @return The memory footprint of the array.
         */
        ReadOnlySpan<MemoryFootprint, 1> GetMemoryFootprint(const char* const debugName) const
        {
            MemoryFootprint footprint = { };
            MemorySystem::CheckMemoryFootprint([&]()
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

        /**
         * @brief Gets a pointer to the data of the array.
         *
         * @return A pointer to the data of the array.
         */
        [[nodiscard]] OTR_INLINE const T* GetData() const { return m_Data; }

        /**
         * @brief Gets the size of the array.
         *
         * @return The size of the array.
         */
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetSize() const { return Size; }

        /**
         * @brief Checks whether the array has been created. An array is created when it has been initialised with
         * a valid size and has not been destroyed.
         *
         * @return True if the array has been created, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsCreated() const { return m_Data && Size > 0; }

        /**
         * @brief Gets an iterator to the first element of the array.
         *
         * @return An iterator to the first element of the array.
         */
        OTR_INLINE Iterator begin() noexcept { return Iterator(m_Data); }

        /**
         * @brief Gets an iterator to the last element of the array.
         *
         * @return An iterator to the last element of the array.
         */
        OTR_INLINE Iterator end() noexcept { return Iterator(m_Data + Size); }

        /**
         * @brief Gets a reverse iterator to the last element of the array.
         *
         * @return A reverse iterator to the last element of the array.
         */
        OTR_INLINE Iterator rbegin() noexcept { return Iterator(m_Data + Size - 1); }

        /**
         * @brief Gets a reverse iterator to the first element of the array.
         *
         * @return A reverse iterator to the first element of the array.
         */
        OTR_INLINE Iterator rend() noexcept { return Iterator(m_Data - 1); }

        /**
         * @brief Gets a const iterator to the first element of the array.
         *
         * @return A const iterator to the first element of the array.
         */
        OTR_INLINE ConstIterator cbegin() const noexcept { return ConstIterator(m_Data); }

        /**
         * @brief Gets a const iterator to the last element of the array.
         *
         * @return A const iterator to the last element of the array.
         */
        OTR_INLINE ConstIterator cend() const noexcept { return ConstIterator(m_Data + Size); }

        /**
         * @brief Gets a reverse const iterator to the last element of the array.
         *
         * @return A reverse const iterator to the last element of the array.
         */
        OTR_INLINE ConstIterator crbegin() const noexcept { return ConstIterator(m_Data + Size - 1); }

        /**
         * @brief Gets a reverse const iterator to the first element of the array.
         *
         * @return A reverse const iterator to the first element of the array.
         */
        OTR_INLINE ConstIterator crend() const noexcept { return ConstIterator(m_Data - 1); }

    private:
        T* m_Data = nullptr;

        friend class ReadOnlyArray<T, Size>;
    };
}

#endif //OTTERENGINE_ARRAY_H
