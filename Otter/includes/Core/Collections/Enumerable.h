#ifndef OTTERENGINE_ENUMERABLE_H
#define OTTERENGINE_ENUMERABLE_H

#include "Core/Memory.h"
#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    /**
     * @brief A simple collection of items that can be iterated over. The items are stored in a contiguous memory block
     * on the heap. It does have a fixed capacity and will not expand as needed.
     *
     * @tparam T The type of the items in the enumerable.
     */
    template<typename T>
    class Enumerable final
    {
        /// @brief An alias for an iterator for the enumerable.
        using Iterator = LinearIterator<T>;

        /// @brief An alias for a const iterator for the enumerable.
        using ConstIterator = LinearIterator<const T>;

    public:
        /**
         * @brief Constructor.
         */
        Enumerable() = default;

        /**
         * @brief Destructor.
         */
        ~Enumerable()
        {
            if (!OwnsData())
                return;

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Count);
        }

        /**
         * @brief Equality operator.
         *
         * @param other The enumerable to compare to.
         *
         * @return True if the enumerables are equal, false otherwise.
         */
        bool operator==(const Enumerable<T>& other) const
        {
            if (m_Count != other.m_Count)
                return false;

            for (UInt64 i = 0; i < m_Count; ++i)
                if (m_Data[i] != other.m_Data[i])
                    return false;

            return true;
        }

        /**
         * @brief Inequality operator.
         *
         * @param other The enumerable to compare to.
         *
         * @return True if the enumerables are not equal, false otherwise.
         */
        bool operator!=(const Enumerable<T>& other) const { return !(*this == other); }

        /**
         * @brief Factory method for creating an enumerable from a list of items.
         *
         * @param list The list of items to create the enumerable from.
         *
         * @return The created enumerable.
         */
        [[nodiscard]] static Enumerable Of(InitialiserList<T> list)
        {
            Enumerable enumerable;

            if (list.size() == 0)
                return enumerable;

            enumerable.m_Data     = Buffer::New<T>(list.size());
            enumerable.m_Count    = 0;
            enumerable.m_OwnsData = true;

            for (const T& item: list)
                enumerable.m_Data[enumerable.m_Count++] = item;

            return enumerable;
        }

        /**
         * @brief Factory method for creating an enumerable by copying an array of items.
         *
         * @param data The pointer to the array of items.
         * @param count The number of items in the array.
         *
         * @return The created enumerable.
         */
        [[nodiscard]] static Enumerable Copy(const T* const data, const UInt64 count)
        {
            Enumerable enumerable;

            enumerable.m_Count = count;

            if (enumerable.m_Count == 0)
                return enumerable;

            enumerable.m_Data = Buffer::New<T>(enumerable.m_Count);
            MemorySystem::MemoryCopy(enumerable.m_Data, data, enumerable.m_Count * sizeof(T));

            enumerable.m_OwnsData = true;

            return enumerable;
        }

        /**
         * @brief Factory method for creating an enumerable that wraps an array of items.
         *
         * @param data The pointer to the array of items.
         * @param count The number of items in the array.
         *
         * @return The created enumerable.
         *
         * @note The enumerable does not own the data and will not delete it when it is destroyed.
         */
        [[nodiscard]] static Enumerable Wrap(T* data, const UInt64 count)
        {
            Enumerable enumerable;

            enumerable.m_Data     = data;
            enumerable.m_Count    = count;
            enumerable.m_OwnsData = false;

            return enumerable;
        }

        /**
         * @brief Clears the enumerable and deletes the data.
         */
        void ClearDestructive()
        {
            if (IsCreated() && OwnsData())
                Buffer::Delete<T>(m_Data, m_Count);

            m_Data  = nullptr;
            m_Count = 0;
        }

        /**
         * @brief Creates an empty enumerable.
         *
         * @return The created enumerable.
         */
        [[nodiscard]] OTR_INLINE static Enumerable Empty() { return { }; }

        /**
         * @brief Gets a pointer to the data of the enumerable.
         *
         * @return A pointer to the data of the enumerable.
         */
        [[nodiscard]] OTR_INLINE const T* GetData() const noexcept { return m_Data; }

        /**
         * @brief Gets the item count of the enumerable.
         *
         * @return The item count of the enumerable.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCount() const noexcept { return m_Count; }

        /**
         * @brief Checks whether the enumerable owns the data.
         *
         * @return True if the enumerable owns the data, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool OwnsData() const noexcept { return m_OwnsData; }

        /**
         * @brief Checks whether the enumerable has been created. An enumerable is created when it has been initialised
         * with a valid count and has not been destroyed.
         *
         * @return True if the enumerable has been created, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept { return m_Data && m_Count > 0; }

        /**
         * @brief Checks whether the enumerable is empty.
         *
         * @return True if the enumerable is empty, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsEmpty() const noexcept { return m_Count == 0; }

        /**
         * @brief Gets an iterator to the first element of the enumerable.
         *
         * @return An iterator to the first element of the enumerable.
         */
        OTR_INLINE Iterator begin() noexcept { return Iterator(m_Data); }

        /**
         * @brief Gets an iterator to the last element of the enumerable.
         *
         * @return An iterator to the last element of the enumerable.
         */
        OTR_INLINE Iterator end() noexcept { return Iterator(m_Data + m_Count); }

        /**
         * @brief Gets a reverse iterator to the last element of the enumerable.
         *
         * @return A reverse iterator to the last element of the enumerable.
         */
        OTR_INLINE Iterator rbegin() noexcept { return Iterator(m_Data + m_Count - 1); }

        /**
         * @brief Gets a reverse iterator to the first element of the enumerable.
         *
         * @return A reverse iterator to the first element of the enumerable.
         */
        OTR_INLINE Iterator rend() noexcept { return Iterator(m_Data - 1); }

        /**
         * @brief Gets a const iterator to the first element of the enumerable.
         *
         * @return A const iterator to the first element of the enumerable.
         */
        OTR_INLINE ConstIterator cbegin() const noexcept { return ConstIterator(m_Data); }

        /**
         * @brief Gets a const iterator to the last element of the enumerable.
         *
         * @return A const iterator to the last element of the enumerable.
         */
        OTR_INLINE ConstIterator cend() const noexcept { return ConstIterator(m_Data + m_Count); }

        /**
         * @brief Gets a reverse const iterator to the last element of the enumerable.
         *
         * @return A reverse const iterator to the last element of the enumerable.
         */
        OTR_INLINE ConstIterator crbegin() const noexcept { return ConstIterator(m_Data + m_Count - 1); }

        /**
         * @brief Gets a reverse const iterator to the first element of the enumerable.
         *
         * @return A reverse const iterator to the first element of the enumerable.
         */
        OTR_INLINE ConstIterator crend() const noexcept { return ConstIterator(m_Data - 1); }

    private:
        T* m_Data = nullptr;
        UInt64 m_Count    = 0;
        bool   m_OwnsData = false;
    };
}

#endif //OTTERENGINE_ENUMERABLE_H
