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
         * @brief Destructor.
         */
        ~Enumerable()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Count);

            m_Data  = nullptr;
            m_Count = 0;
        }

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
            enumerable.m_Data = Buffer::New<T>(list.size());

            enumerable.m_Count = 0;
            for (const T& item: list)
                enumerable.m_Data[enumerable.m_Count++] = item;

            return enumerable;
        }

        /**
         * @brief Clears the enumerable and deletes the data.
         */
        void ClearDestructive()
        {
            if (IsCreated())
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
        OTR_INLINE ConstIterator begin() const noexcept { return ConstIterator(m_Data); }

        /**
         * @brief Gets a const iterator to the last element of the enumerable.
         *
         * @return A const iterator to the last element of the enumerable.
         */
        OTR_INLINE ConstIterator end() const noexcept { return ConstIterator(m_Data + m_Count); }

        /**
         * @brief Gets a reverse const iterator to the last element of the enumerable.
         *
         * @return A reverse const iterator to the last element of the enumerable.
         */
        OTR_INLINE ConstIterator rbegin() const noexcept { return ConstIterator(m_Data + m_Count - 1); }

        /**
         * @brief Gets a reverse const iterator to the first element of the enumerable.
         *
         * @return A reverse const iterator to the first element of the enumerable.
         */
        OTR_INLINE ConstIterator rend() const noexcept { return ConstIterator(m_Data - 1); }

    private:
        /**
         * @brief Constructor.
         */
        Enumerable() = default;

        T* m_Data = nullptr;
        UInt64 m_Count = 0;
    };
}

#endif //OTTERENGINE_ENUMERABLE_H
