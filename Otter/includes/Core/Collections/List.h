#ifndef OTTERENGINE_LIST_H
#define OTTERENGINE_LIST_H

#include "Core/Collections/Collection.h"
#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    /**
     * @brief A list of items. The items are stored in a contiguous memory block on the heap. The difference between
     * a List<T> and an Array<T, Size> is that a List does not have a fixed capacity. It will expand as needed.
     *
     * @tparam T The type of the items in the list.
     */
    template<typename T>
    class List final : public Collection<T>
    {
        OTR_USING_BASE(Collection<T>)

        /// @brief An alias for an iterator for the list.
        using Iterator = LinearIterator<T>;

        /// @brief An alias for a const iterator for the list.
        using ConstIterator = LinearIterator<const T>;

    public:
        OTR_COLLECTION_CONSTRUCT(List)
        OTR_COLLECTION_COPY(List)
        OTR_COLLECTION_MOVE(List)

        /**
         * @brief Gets the element at the specified index.
         *
         * @param index The index.
         *
         * @return The element at the specified index.
         */
        [[nodiscard]] T& operator[](const UInt64 index)
        {
            OTR_ASSERT(index < base::m_Count, "List index out of bounds")
            return base::m_Data[index];
        }

        /**
         * @brief Gets the element at the specified index.
         *
         * @param index The index.
         *
         * @return The element at the specified index.
         */
        [[nodiscard]] const T& operator[](const UInt64 index) const
        {
            OTR_ASSERT(index < base::m_Count, "List index out of bounds")
            return base::m_Data[index];
        }

        /**
         * @brief Adds an item to the list.
         *
         * @param item The item to add.
         */
        void Add(const T& item)
        {
            if (base::m_Count >= base::m_Capacity)
                base::Expand();

            base::m_Data[base::m_Count++] = item;
        }

        /**
         * @brief Adds an item to the list.
         *
         * @param item The item to add.
         */
        void Add(T&& item) noexcept
        {
            if (base::m_Count >= base::m_Capacity)
                base::Expand();

            base::m_Data[base::m_Count++] = std::move(item);
        }

        /**
         * @brief Tries to add an item to the list at the specified index.
         *
         * @param index The index at which to add the item.
         * @param item The item to add.
         *
         * @return True if the item was added, false otherwise.
         */
        bool TryAddAt(const UInt64 index, const T& item)
        {
            if (index >= base::m_Capacity || base::m_Count >= base::m_Capacity)
                return false;

            for (UInt64 i = base::m_Count; i > index; i--)
                base::m_Data[i] = base::m_Data[i - 1];

            base::m_Data[index] = item;
            base::m_Count++;

            return true;
        }

        /**
         * @brief Tries to add an item to the list at the specified index.
         *
         * @param index The index at which to add the item.
         * @param item The item to add.
         *
         * @return True if the item was added, false otherwise.
         */
        bool TryAddAt(const UInt64 index, T&& item)
        {
            if (index >= base::m_Capacity || base::m_Count >= base::m_Capacity)
                return false;

            for (UInt64 i = base::m_Count; i > index; i--)
                base::m_Data[i] = base::m_Data[i - 1];

            base::m_Data[index] = std::move(item);
            base::m_Count++;

            return true;
        }

        /**
         * @brief Tries to add a range of items to the list.
         *
         * @param items The items to add.
         * @param allOrNothing Whether or not to add all items or none at all, if there is not enough space.
         *
         * @return True if the items were added, false otherwise.
         */
        bool TryAddRange(InitialiserList<T> items, bool allOrNothing = false)
        {
            return TryAddRangeInternal(items.begin(), items.size(), allOrNothing);
        }

        /**
         * @brief Tries to add a collection of items to the list.
         *
         * @param collection The items to add.
         * @param allOrNothing Whether or not to add all items or none at all, if there is not enough space.
         *
         * @return True if the items were added, false otherwise.
         */
        bool TryAddRange(const Collection<T>& collection, bool allOrNothing = false)
        {
            return TryAddRangeInternal(collection.GetData(), collection.GetCount(), allOrNothing);
        }

        /**
         * @brief Tries to remove an item from the list.
         *
         * @param item The item to remove.
         *
         * @return True if the item was removed, false otherwise.
         */
        bool TryRemove(const T& item)
        {
            for (UInt64 i = 0; i < base::m_Count; i++)
                if (base::m_Data[i] == item)
                    return TryRemoveAt(i);

            return false;
        }

        /**
         * @brief Tries to remove an item from the list at the specified index.
         *
         * @param index The index at which to remove the item.
         *
         * @return True if the item was removed, false otherwise.
         */
        bool TryRemoveAt(const UInt64 index)
        {
            if (index >= base::m_Count)
                return false;

            if (index != base::m_Count - 1)
                base::m_Data[index] = std::move(base::m_Data[base::m_Count - 1]);

            base::m_Count--;

            return true;
        }

        /**
         * @brief Gets an iterator to the first element of the list.
         *
         * @return An iterator to the first element of the list.
         */
        OTR_INLINE Iterator begin() const noexcept { return Iterator(base::m_Data); }

        /**
         * @brief Gets an iterator to the last element of the list.
         *
         * @return An iterator to the last element of the list.
         */
        OTR_INLINE Iterator end() const noexcept { return Iterator(base::m_Data + base::m_Count); }

        /**
         * @brief Gets a reverse iterator to the last element of the list.
         *
         * @return A reverse iterator to the last element of the list.
         */
        OTR_INLINE Iterator rbegin() const noexcept { return Iterator(base::m_Data + base::m_Count - 1); }

        /**
         * @brief Gets a reverse iterator to the first element of the list.
         *
         * @return A reverse iterator to the first element of the list.
         */
        OTR_INLINE Iterator rend() const noexcept { return Iterator(base::m_Data - 1); }

        /**
         * @brief Gets a const iterator to the first element of the list.
         *
         * @return A const iterator to the first element of the list.
         */
        OTR_INLINE ConstIterator cbegin() const noexcept { return ConstIterator(base::m_Data); }

        /**
         * @brief Gets a const iterator to the last element of the list.
         *
         * @return A const iterator to the last element of the list.
         */
        OTR_INLINE ConstIterator cend() const noexcept { return ConstIterator(base::m_Data + base::m_Count); }

        /**
         * @brief Gets a reverse const iterator to the last element of the list.
         *
         * @return A reverse const iterator to the last element of the list.
         */
        OTR_INLINE ConstIterator crbegin() const noexcept { return ConstIterator(base::m_Data + base::m_Count - 1); }

        /**
         * @brief Gets a reverse const iterator to the first element of the list.
         *
         * @return A reverse const iterator to the first element of the list.
         */
        OTR_INLINE ConstIterator crend() const noexcept { return ConstIterator(base::m_Data - 1); }

    private:
        /**
         * @brief Tries to add data to the list.
         *
         * @param data The data to add.
         * @param size The size of the data to add.
         * @param allOrNothing Whether or not to add all data or none at all, if there is not enough space.
         *
         * @return True if the data were added, false otherwise.
         */
        bool TryAddRangeInternal(const void* data, const UInt64 size, bool allOrNothing = false)
        {
            if (!data || size == 0)
                return false;

            if (size > base::m_Capacity - base::m_Count)
            {
                if (allOrNothing)
                    return false;

                base::Expand(size - (base::m_Capacity - base::m_Count));
            }

            const UInt64 offset = sizeof(T);
            MemorySystem::MemoryCopy(base::m_Data + (base::m_Count * offset), data, (size * offset));
            base::m_Count += size;

            return true;
        }
    };
}

#endif //OTTERENGINE_LIST_H
