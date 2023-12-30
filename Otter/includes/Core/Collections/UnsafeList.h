#ifndef OTTERENGINE_UNSAFELIST_H
#define OTTERENGINE_UNSAFELIST_H

#include "Core/Memory.h"
#include "Core/Collections/Collection.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    /**
     * @brief An unsafe list of items. The items are stored in a contiguous memory block on the heap. Use only when
     * the type of the items is known beforehand but the use of a templated list is not wanted.
     */
    class UnsafeList final
    {
    public:
        /**
         * @brief Constructor.
         */
        UnsafeList() = default;

        /**
         * @brief Destructor.
         */
        ~UnsafeList()
        {
            if (IsCreated())
                Unsafe::Delete({ m_Data, m_Capacity * m_Offset });

            m_Data = nullptr;
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The unsafe list to copy.
         */
        UnsafeList(const UnsafeList& other)
        {
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Offset   = other.m_Offset;

            if (m_Capacity == 0)
                return;

            auto handle = Unsafe::New(m_Capacity * m_Offset);
            m_Data = (Byte*) handle.Pointer;

            if (other.IsCreated() && !other.IsEmpty())
                MemorySystem::MemoryCopy(m_Data, other.m_Data, m_Count * m_Offset);
        }

        /**
         * @brief Move constructor.
         *
         * @param other The unsafe list to move.
         */
        UnsafeList(UnsafeList&& other) noexcept
        {
            m_Data     = other.m_Data;
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Offset   = other.m_Offset;

            other.m_Data     = nullptr;
            other.m_Capacity = 0;
            other.m_Count    = 0;
            other.m_Offset   = 0;
        }

        /**
         * @brief Copy assignment operator.
         */
        UnsafeList& operator=(const UnsafeList& other)
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Unsafe::Delete({ m_Data, m_Capacity * m_Offset });

            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Offset   = other.m_Offset;

            if (m_Capacity == 0)
                return *this;

            auto handle = Unsafe::New(m_Capacity * m_Offset);
            m_Data = (Byte*) handle.Pointer;

            if (other.IsCreated() && !other.IsEmpty())
                MemorySystem::MemoryCopy(m_Data, other.m_Data, m_Count * m_Offset);

            return *this;
        }

        /**
         * @brief Move assignment operator.
         */
        UnsafeList& operator=(UnsafeList&& other) noexcept
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Unsafe::Delete({ m_Data, m_Capacity * m_Offset });

            m_Data     = other.m_Data;
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Offset   = other.m_Offset;

            other.m_Data     = nullptr;
            other.m_Capacity = 0;
            other.m_Count    = 0;
            other.m_Offset   = 0;

            return *this;
        }

        /**
         * @brief Equality operator.
         *
         * @param other The unsafe list to compare to.
         *
         * @return True if the buffers are equal, false otherwise.
         */
        [[nodiscard]] bool operator==(const UnsafeList& other) const
        {
            return m_Offset == other.m_Offset
                   && m_Data == other.m_Data
                   && m_Capacity == other.m_Capacity
                   && m_Count == other.m_Count;
        }

        /**
         * @brief Inequality operator.
         *
         * @param other The unsafe list to compare to.
         *
         * @return True if the buffers are not equal, false otherwise.
         */
        [[nodiscard]] bool operator!=(const UnsafeList& other) const { return !(*this == other); }

        /**
         * @brief Gets the item at the specified index.
         *
         * @param index The index.
         *
         * @return The address of the item at the specified index.
         *
         * @note This operator is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        template<typename T>
        requires std::is_trivially_destructible_v<T>
        [[nodiscard]] T* operator[](const UInt64 index) const
        {
            OTR_ASSERT_MSG(index < m_Count, "Index out of range")

            return reinterpret_cast<T*>(m_Data + (index * m_Offset));
        }

        /**
         * @brief Creates an empty unsafe list.
         *
         * @tparam T The type of an item in the list.
         *
         * @return The unsafe list.
         */
        template<typename T>
        requires std::is_trivially_destructible_v<T>
        [[nodiscard]] OTR_INLINE static UnsafeList Empty() { return UnsafeList(sizeof(T)); }

        /**
         * @brief Creates an unsafe list from an initialiser list.
         *
         * @tparam T The type of an item in the initialiser list.
         *
         * @param list The initialiser list.
         *
         * @return The unsafe list.
         */
        template<typename T>
        requires std::is_trivially_destructible_v<T>
        [[nodiscard]] static UnsafeList Of(InitialiserList<T> list)
        {
            auto unsafeList = UnsafeList(sizeof(T));
            unsafeList.m_Capacity = list.size();
            unsafeList.m_Count    = list.size();

            if (unsafeList.m_Capacity == 0)
                return unsafeList;

            auto handle = Unsafe::New(unsafeList.m_Capacity * unsafeList.m_Offset);
            unsafeList.m_Data = (Byte*) handle.Pointer;

            MemorySystem::MemoryCopy(unsafeList.m_Data, list.begin(), unsafeList.m_Count * unsafeList.m_Offset);

            return unsafeList;
        }

        /**
         * @brief Gets the item at the specified index.
         *
         * @tparam T The type of the item.
         *
         * @param index The index.
         * @param outItem The item at the specified index.
         *
         * @return True if the item was retrieved, false otherwise.
         *
         * @note This function is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        template<typename T>
        requires std::is_trivially_destructible_v<T>
        [[nodiscard]] bool TryGet(const UInt64 index, T* outItem) const
        {
            OTR_ASSERT_MSG(outItem, "Out item must not be null")

            if (index >= m_Count)
                return false;

            *outItem = *reinterpret_cast<const T*>(m_Data + (index * m_Offset));

            return true;
        }

        /**
         * @brief Adds an item to the buffer.
         *
         * @tparam T The type of the item.
         *
         * @param item The item to add.
         *
         * @note This function is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        template<typename T>
        requires std::is_trivially_destructible_v<T>
        void Add(const T& item)
        {
            if (m_Count >= m_Capacity)
                Expand();

            MemorySystem::MemoryCopy((void*) (m_Data + (m_Count * m_Offset)), (void*) &item, m_Offset);
            m_Count++;
        }

        /**
         * @brief Adds an item to the buffer.
         *
         * @tparam T The type of the item.
         *
         * @param item The item to add.
         *
         * @note This function is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        template<typename T>
        requires std::is_trivially_destructible_v<T>
        void Add(T&& item)
        {
            if (m_Count >= m_Capacity)
                Expand();

            MemorySystem::MemoryCopy((void*) (m_Data + (m_Count * m_Offset)), (void*) &item, m_Offset);
            m_Count++;
        }

        /**
         * @brief Tries to add an item to the list at the specified index.
         *
         * @tparam T The type of the item.
         *
         * @param index The index at which to add the item.
         * @param item The item to add.
         *
         * @return True if the item was added, false otherwise.
         *
         * @note This function is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        template<typename T>
        requires std::is_trivially_destructible_v<T>
        bool TryAddAt(const UInt64 index, const T& item)
        {
            if (index >= m_Capacity - 1 || m_Count >= m_Capacity - 1)
                return false;

            MemorySystem::MemoryMove(m_Data + ((index + 1) * m_Offset),
                                     m_Data + (index * m_Offset),
                                     (m_Count - index) * m_Offset);

            MemorySystem::MemoryCopy(m_Data + (index * m_Offset), &item, m_Offset);
            m_Count++;

            return true;
        }

        /**
         * @brief Tries to add an item to the list at the specified index.
         *
         * @tparam T The type of the item.
         *
         * @param index The index at which to add the item.
         * @param item The item to add.
         *
         * @return True if the item was added, false otherwise.
         *
         * @note This function is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        template<typename T>
        requires std::is_trivially_destructible_v<T>
        bool TryAddAt(const UInt64 index, T&& item)
        {
            if (index >= m_Capacity || m_Count >= m_Capacity)
                return false;

            MemorySystem::MemoryMove(m_Data + ((index + 1) * m_Offset),
                                     m_Data + (index * m_Offset),
                                     (m_Count - index) * m_Offset);

            MemorySystem::MemoryCopy(m_Data + (index * m_Offset), &item, m_Offset);
            m_Count++;

            return true;
        }

        /**
         * @brief Tries to add an unsafe list to the buffer.
         *
         * @param list The list of items to add.
         * @param allOrNothing Whether or not to add all items or none at all, if there is not enough space.
         *
         * @return True if the items were added, false otherwise.
         *
         * @note This function is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        bool TryAddRange(const UnsafeList& list, bool allOrNothing = false)
        {
            OTR_ASSERT_MSG(list.GetOffset() == m_Offset, "Size of type must be equal to the offset of the list")

            return TryAddRangeInternal(list.GetData(), list.GetCount(), allOrNothing);
        }

        /**
         * @brief Tries to add a range of items to the buffer.
         *
         * @tparam T The type of the item.
         *
         * @param items The items to add.
         * @param allOrNothing Whether or not to add all items or none at all, if there is not enough space.
         *
         * @return True if the items were added, false otherwise.
         *
         * @note This function is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        template<typename T>
        requires std::is_trivially_destructible_v<T>
        bool TryAddRange(InitialiserList<T> items, bool allOrNothing = false)
        {
            return TryAddRangeInternal(items.begin(), items.size(), allOrNothing);
        }

        /**
         * @brief Tries to add a collection of items to the buffer.
         *
         * @tparam T The type of the item.
         *
         * @param collection The collection to add.
         * @param allOrNothing Whether or not to add all items or none at all, if there is not enough space.
         *
         * @return True if the items were added, false otherwise.
         *
         * @note This function is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        template<typename T>
        requires std::is_trivially_destructible_v<T>
        bool TryAddRange(const Collection<T>& collection, bool allOrNothing = false)
        {
            return TryAddRangeInternal(collection.GetData(), collection.GetCount(), allOrNothing);
        }

        /**
         * @brief Tries to remove an item from the buffer.
         *
         * @tparam T The type of the item.
         *
         * @param item The item to remove.
         *
         * @return True if the item was removed, false otherwise.
         *
         * @note This function is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        template<typename T>
        requires std::is_trivially_destructible_v<T>
        bool TryRemove(const T& item)
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (*reinterpret_cast<T*>(m_Data + (i * m_Offset)) == item)
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
            if (index >= m_Count)
                return false;

            if (index != m_Count - 1)
                MemorySystem::MemoryMove(m_Data + (index * m_Offset),
                                         m_Data + ((index + 1) * m_Offset),
                                         (m_Count - index - 1) * m_Offset);

            m_Count--;

            return true;
        }

        /**
         * @brief Checks if the list contains a given item.
         *
         * @tparam T The type of the item.
         *
         * @param item The item to check for.
         *
         * @return True if the list contains the item, false otherwise.
         *
         * @note This function is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        template<typename T>
        requires std::is_trivially_destructible_v<T>
        [[nodiscard]] bool Contains(const T& item) const
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (*reinterpret_cast<T*>(m_Data + (i * m_Offset)) == item)
                    return true;

            return false;
        }

        /**
         * @brief Tries to get the index of a given item.
         *
         * @param item The item to get the index of.
         * @param outIndex The index of the item.
         *
         * @return True if the item was found, false otherwise.
         *
         * @note This function is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        template<typename T>
        requires std::is_trivially_destructible_v<T>
        [[nodiscard]] bool TryGetIndexOf(const T& item, UInt64* outIndex) const
        {
            OTR_ASSERT_MSG(outIndex, "Out index must not be null")

            for (UInt64 i = 0; i < m_Count; i++)
                if (*reinterpret_cast<T*>(m_Data + (i * m_Offset)) == item)
                {
                    *outIndex = i;
                    return true;
                }

            return false;
        }

        /**
         * @brief Used to reserve space for the buffer.
         *
         * @param capacity The capacity to reserve.
         *
         * @note This operation is destructive and will delete any existing data.
         */
        void Reserve(const UInt64 capacity)
        {
            RecreateEmpty(capacity);
        }

        /**
         * @brief Used to expand the size of the list by a given amount.
         *
         * @param amount The amount to expand the list by.
         */
        void Expand(const UInt64 amount = 0)
        {
            UInt64 newCapacity = CalculateExpandCapacity(amount);

            auto handle = Unsafe::New(newCapacity * m_Offset);

            if (IsCreated())
            {
                if (!IsEmpty())
                    MemorySystem::MemoryCopy(handle.Pointer, m_Data, m_Count * m_Offset);

                Unsafe::Delete({ m_Data, m_Capacity * m_Offset });
            }

            m_Data     = (Byte*) handle.Pointer;
            m_Capacity = newCapacity;
        }

        /**
         * @brief Used to shrink the size of the list by a given amount.
         *
         * @param amount The amount to shrink the list by.
         * @param isDestructive Whether or not the shrink is destructive. If true, some data may be lost.
         */
        void Shrink(const UInt64 amount = 0, const bool isDestructive = false)
        {
            UInt64 newCapacity = CalculateShrinkCapacity(amount, isDestructive);

            if (IsEmpty() || newCapacity == 0)
            {
                RecreateEmpty(newCapacity);
                return;
            }

            auto handle = Unsafe::New(newCapacity * m_Offset);

            if (IsCreated())
            {
                auto minCount = m_Count < newCapacity
                                ? m_Count
                                : newCapacity;

                MemorySystem::MemoryCopy(handle.Pointer, m_Data, minCount * m_Offset);
                Unsafe::Delete({ m_Data, m_Capacity * m_Offset });
            }

            m_Data     = (Byte*) handle.Pointer;
            m_Capacity = newCapacity;

            if (m_Count >= newCapacity)
                m_Count = newCapacity;
        }

        /**
         * @brief Clears the buffer.
         */
        OTR_INLINE void Clear() { m_Count = 0; }

        /**
         * @brief Clears the list and deletes the data.
         */
        void ClearDestructive()
        {
            if (IsCreated())
                Unsafe::Delete({ m_Data, m_Capacity * m_Offset });

            m_Data     = nullptr;
            m_Capacity = 0;
            m_Count    = 0;
        }

#if !OTR_RUNTIME
        /**
         * @brief Gets the memory footprint of the buffer.
         *
         * @param debugName The name of the list for debugging purposes.
         *
         * @return The memory footprint of the buffer.
         */
        [[nodiscard]] ReadOnlySpan<MemoryFootprint, 1> GetMemoryFootprint(const char* const debugName) const
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
         * @brief Gets a pointer to the data of the buffer.
         *
         * @tparam T The type of the data.
         *
         * @return A pointer to the data of the list cast to the specified type.
         *
         * @note This function is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        template<typename T = void>
        [[nodiscard]] OTR_INLINE const T* GetData() const noexcept
        {
            if constexpr (IsVoid<T>)
                return (void*) m_Data;
            else
                return reinterpret_cast<const T*>(m_Data);
        }

        /**
         * @brief Gets the capacity of the buffer.
         *
         * @return The capacity of the buffer.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCapacity() const noexcept { return m_Capacity; }

        /**
         * @brief Gets the item count of the buffer.
         *
         * @return The item count of the buffer.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCount() const noexcept { return m_Count; }

        /**
         * @brief Gets the offset of elements in the buffer.
         *
         * @return The offset of elements in the buffer.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetOffset() const noexcept { return m_Offset; }

        /**
         * @brief Checks whether the list has been created. A list is created when it has been initialised
         * with a valid capacity and has not been destroyed.
         *
         * @return True if the list has been created, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept { return m_Data && m_Capacity > 0; }

        /**
         * @brief Checks whether the list is empty.
         *
         * @return True if the list is empty, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsEmpty() const noexcept { return m_Count == 0; }

    private:
        /**
         * @brief Constructor.
         *
         * @param offset The offset of each item in the buffer.
         */
        explicit UnsafeList(const UInt64 offset)
            : m_Offset(offset)
        {
            OTR_ASSERT_MSG(offset > 0, "The offset of the list items must be greater than 0.")
        }

        Byte* m_Data = nullptr;
        UInt64 m_Count    = 0;
        UInt64 m_Capacity = 0;
        UInt64 m_Offset   = 0;

        /**
         * @brief Tries to add data to the buffer.
         *
         * @param data The data to add.
         * @param size The size of the data to add.
         * @param allOrNothing Whether or not to add all data or none at all, if there is not enough space.
         *
         * @return True if the data were added, false otherwise.
         *
         * @note This function is unsafe and should be used with caution. It is assumed that the developer already
         * knows the type beforehand.
         */
        bool TryAddRangeInternal(const void* const data, const UInt64 size, bool allOrNothing = false)
        {
            if (!data || size == 0)
                return false;

            if (size > m_Capacity - m_Count)
            {
                if (allOrNothing)
                    return false;

                Expand(size - (m_Capacity - m_Count));
            }

            MemorySystem::MemoryCopy(m_Data + (m_Count * m_Offset), data, size * m_Offset);
            m_Count += size;

            return true;
        }

        /**
         * @brief Recreates the list with a given capacity. Deletes any existing data.
         *
         * @param capacity The capacity to recreate the list with.
         */
        void RecreateEmpty(const UInt64 capacity)
        {
            if (IsCreated())
                Unsafe::Delete({ m_Data, m_Capacity * m_Offset });

            m_Capacity = capacity;
            m_Count    = 0;

            if (m_Capacity == 0)
                return;

            auto handle = Unsafe::New(m_Capacity * m_Offset);
            m_Data = (Byte*) handle.Pointer;
        }

        /**
         * @brief Calculates the new capacity when expanding the buffer.
         *
         * @param expandAmount The amount to expand the list by.
         *
         * @return The new capacity.
         */
        [[nodiscard]] UInt64 CalculateExpandCapacity(const UInt64 expandAmount) const
        {
            UInt64 newCapacity;

            if (expandAmount == 0)
                newCapacity = m_Capacity == 0 ? 2 : m_Capacity * 1.5;
            else
                newCapacity = m_Capacity + expandAmount;

            return newCapacity;
        }

        /**
         * @brief Calculates the new capacity when shrinking the buffer.
         *
         * @param shrinkAmount The amount to shrink the list by.
         * @param isDestructive Whether or not the shrink is destructive. If true, some data may be lost.
         *
         * @return The new capacity.
         */
        [[nodiscard]] UInt64 CalculateShrinkCapacity(const UInt64 shrinkAmount, const bool isDestructive) const
        {
            if (m_Capacity == 0)
                return 0;

            UInt64 newCapacity;

            if (shrinkAmount == 0)
                newCapacity = m_Capacity * 0.75;
            else if (shrinkAmount > m_Capacity)
                newCapacity = 0;
            else
                newCapacity = m_Capacity - shrinkAmount;

            const auto currentCount = GetCount();

            if (!isDestructive && newCapacity < currentCount)
                newCapacity = currentCount;

            return newCapacity;
        }
    };
}

#endif //OTTERENGINE_UNSAFELIST_H
