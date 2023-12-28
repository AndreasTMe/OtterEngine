#ifndef OTTERENGINE_READONLYARRAY_H
#define OTTERENGINE_READONLYARRAY_H

#include "Core/Memory.h"
#include "Core/Collections/Iterators/LinearIterator.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    template<typename T, UInt64 Size>
    class Array;

    /**
     * @brief A read-only array of a fixed size. All elements are heap allocated.
     *
     * @tparam T The type of the elements in the array.
     * @tparam Size The size of the array.
     */
    template<typename T, UInt64 Size>
    class ReadOnlyArray final
    {
        /// @brief Alias for a const iterator.
        using ConstIterator = LinearIterator<const T>;

    public:
        /**
         * @brief Constructor.
         */
        ReadOnlyArray()
        {
            if constexpr (Size > 0)
                m_Data = Buffer::New<T>(Size);
        }

        /**
         * @brief Destructor.
         */
        ~ReadOnlyArray()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, Size);

            m_Data = nullptr;
        }

        /**
         * @brief Deleted copy constructor.
         */
        ReadOnlyArray(ReadOnlyArray&) = delete;

        /**
         * @brief Deleted copy constructor.
         */
        ReadOnlyArray(const ReadOnlyArray&) = delete;

        /**
         * @brief Deleted copy assignment operator.
         */
        ReadOnlyArray& operator=(const ReadOnlyArray&) = delete;

        /**
         * @brief Deleted move constructor.
         */
        ReadOnlyArray(ReadOnlyArray&&) = delete;

        /**
         * @brief Deleted move assignment operator.
         */
        ReadOnlyArray& operator=(ReadOnlyArray&&) = delete;

        /**
         * @brief Equality operator.
         *
         * @param other The array to compare to.
         *
         * @return True if the arrays are equal, false otherwise.
         */
        [[nodiscard]] bool operator==(const ReadOnlyArray<T, Size>& other) const { return m_Data == other.m_Data; }

        /**
         * @brief Inequality operator.
         *
         * @param other The array to compare to.
         *
         * @return True if the arrays are not equal, false otherwise.
         */
        [[nodiscard]] bool operator!=(const ReadOnlyArray<T, Size>& other) const { return !(*this == other); }

        /**
         * @brief Creates a read-only array from an initialiser list.
         *
         * @param list The initialiser list.
         */
        ReadOnlyArray(InitialiserList<T> list)
            : ReadOnlyArray()
        {
            OTR_ASSERT_MSG(list.size() == Size, "Initialiser list size does not match span size")

            UInt64 i = 0;
            for (const T& value: list)
                m_Data[i++] = value;
        }

        /**
         * @brief Creates a read-only array by copying a normal array.
         *
         * @param other The normal array.
         */
        explicit ReadOnlyArray(const Array<T, Size>& other)
            : ReadOnlyArray()
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];
        }

        /**
         * @brief Creates a read-only array by moving a normal array.
         *
         * @param other The normal array.
         */
        explicit ReadOnlyArray(Array<T, Size>&& other) noexcept
            : ReadOnlyArray()
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);

            other.m_Data = nullptr;
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
            OTR_ASSERT_MSG(index < Size, "ReadOnlyArray index out of bounds")
            return m_Data[index];
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
        T* m_Data;
    };
}

#endif //OTTERENGINE_READONLYARRAY_H
