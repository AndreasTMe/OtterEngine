#ifndef OTTERENGINE_UNSAFEBUFFER_H
#define OTTERENGINE_UNSAFEBUFFER_H

#include <typeindex>

#include "Core/Memory.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    /**
     * @brief Unsafe buffer class that is used to store data in a contiguous memory block.
     */
    class UnsafeBuffer final
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param offset The offset of each item in the buffer.
         */
        explicit UnsafeBuffer(const UInt64 offset)
            : k_Offset(OTR_ALIGNED_OFFSET(offset, OTR_PLATFORM_MEMORY_ALIGNMENT))
        {
            OTR_ASSERT_MSG(offset > 0, "The offset of the buffer must be greater than 0.")
        }

        /**
         * @brief Destructor.
         */
        ~UnsafeBuffer()
        {
            ClearDestructive();
        }

        /**
         * @brief Creates a buffer from an initialiser list.
         *
         * @tparam T The type of an item in the initialiser list.
         *
         * @param list The initialiser list.
         */
        template<typename T>
        UnsafeBuffer(InitialiserList<T> list)
            : UnsafeBuffer(sizeof(T))
        {
            m_Capacity = list.size();
            m_Count    = list.size();

            if (m_Capacity == 0)
                return;

            auto handle = Unsafe::New(m_Capacity * k_Offset);
            m_Data = handle.Pointer;

            MemorySystem::MemoryCopy(m_Data, list.begin(), m_Count * k_Offset);
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The buffer to copy.
         */
        UnsafeBuffer(const UnsafeBuffer& other)
            : k_Offset(other.k_Offset)
        {
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;

            auto handle = Unsafe::New(m_Capacity * k_Offset);
            m_Data = handle.Pointer;

            if (other.IsCreated() && !other.IsEmpty())
                MemorySystem::MemoryCopy(m_Data, other.m_Data, m_Count * k_Offset);
        }

        /**
         * @brief Move constructor.
         *
         * @param other The buffer to move.
         */
        UnsafeBuffer(UnsafeBuffer&& other) noexcept
            : k_Offset(other.k_Offset)
        {
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Data     = other.m_Data;

            other.m_Capacity = 0;
            other.m_Count    = 0;
            other.m_Data     = nullptr;
        }

        /**
         * @brief Deleted copy assignment operator.
         */
        UnsafeBuffer& operator=(const UnsafeBuffer& other) = delete;

        /**
         * @brief Deleted move assignment operator.
         */
        UnsafeBuffer& operator=(UnsafeBuffer&& other) noexcept = delete;

        /**
         * @brief Equality operator.
         *
         * @param other The buffer to compare to.
         *
         * @return True if the buffers are equal, false otherwise.
         */
        [[nodiscard]] bool operator==(const UnsafeBuffer& other) const
        {
            return k_Offset == other.k_Offset
                   && m_Data == other.m_Data
                   && m_Capacity == other.m_Capacity
                   && m_Count == other.m_Count;
        }

        /**
         * @brief Inequality operator.
         *
         * @param other The buffer to compare to.
         *
         * @return True if the buffers are not equal, false otherwise.
         */
        [[nodiscard]] bool operator!=(const UnsafeBuffer& other) const { return !(*this == other); }

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
         * @note The T type is checked against the offset of the buffer to ensure that the type is correct. However,
         * this check does not guarantee that the type is correct. Ultimately, this function is unsafe and should be
         * used with caution.
         */
        template<typename T>
        [[nodiscard]] bool TryGet(const UInt64 index, T* outItem) const
        {
            OTR_ASSERT_MSG(sizeof(T) == k_Offset, "The type of the buffer does not match the offset of the buffer.")
            OTR_ASSERT_MSG(outItem, "Out item must not be null")

            if (index >= m_Count)
                return false;

            *outItem = *reinterpret_cast<const T*>((Byte*) m_Data + (index * k_Offset));

            return true;
        }

        /**
         * @brief Adds an item to the buffer.
         *
         * @tparam T The type of the item.
         *
         * @param item The item to add.
         *
         * @note The T type is checked against the offset of the buffer to ensure that the type is correct. However,
         * this check does not guarantee that the type is correct. Ultimately, this function is unsafe and should be
         * used with caution.
         */
        template<typename T>
        void Add(const T& item)
        {
            OTR_ASSERT_MSG(sizeof(T) == k_Offset, "The type of the buffer does not match the offset of the buffer.")

            if (m_Count >= m_Capacity)
                Expand();

            MemorySystem::MemoryCopy((Byte*) m_Data + (m_Count * k_Offset), &item, k_Offset);
            m_Count++;
        }

        /**
         * @brief Adds an item to the buffer.
         *
         * @tparam T The type of the item.
         *
         * @param item The item to add.
         *
         * @note The T type is checked against the offset of the buffer to ensure that the type is correct. However,
         * this check does not guarantee that the type is correct. Ultimately, this function is unsafe and should be
         * used with caution.
         */
        template<typename T>
        void Add(T&& item)
        {
            OTR_ASSERT_MSG(sizeof(T) == k_Offset, "The type of the buffer does not match the offset of the buffer.")

            if (m_Count >= m_Capacity)
                Expand();

            MemorySystem::MemoryCopy((Byte*) m_Data + (m_Count * k_Offset), &std::forward<T>(item), k_Offset);
            m_Count++;
        }

        /**
         * @brief Tries to add an item to the buffer at the specified index.
         *
         * @tparam T The type of the item.
         *
         * @param index The index at which to add the item.
         * @param item The item to add.
         *
         * @return True if the item was added, false otherwise.
         *
         * @note The T type is checked against the offset of the buffer to ensure that the type is correct. However,
         * this check does not guarantee that the type is correct. Ultimately, this function is unsafe and should be
         * used with caution.
         */
        template<typename T>
        bool TryAddAt(const UInt64 index, const T& item)
        {
            OTR_ASSERT_MSG(sizeof(T) == k_Offset, "The type of the buffer does not match the offset of the buffer.")

            if (index >= m_Capacity || m_Count >= m_Capacity)
                return false;

            MemorySystem::MemoryMove((Byte*) m_Data + ((index + 1) * k_Offset),
                                     (Byte*) m_Data + (index * k_Offset),
                                     (m_Count - index) * k_Offset);

            MemorySystem::MemoryCopy((Byte*) m_Data + (index * k_Offset), &item, k_Offset);
            m_Count++;

            return true;
        }

        /**
         * @brief Tries to add an item to the buffer at the specified index.
         *
         * @tparam T The type of the item.
         *
         * @param index The index at which to add the item.
         * @param item The item to add.
         *
         * @return True if the item was added, false otherwise.
         *
         * @note The T type is checked against the offset of the buffer to ensure that the type is correct. However,
         * this check does not guarantee that the type is correct. Ultimately, this function is unsafe and should be
         * used with caution.
         */
        template<typename T>
        bool TryAddAt(const UInt64 index, T&& item)
        {
            OTR_ASSERT_MSG(sizeof(T) == k_Offset, "The type of the buffer does not match the offset of the buffer.")

            if (index >= m_Capacity || m_Count >= m_Capacity)
                return false;

            MemorySystem::MemoryMove((Byte*) m_Data + ((index + 1) * k_Offset),
                                     (Byte*) m_Data + (index * k_Offset),
                                     (m_Count - index) * k_Offset);

            MemorySystem::MemoryCopy((Byte*) m_Data + (index * k_Offset), &std::forward<T>(item), k_Offset);
            m_Count++;

            return true;
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
         * @note The T type is checked against the offset of the buffer to ensure that the type is correct. However,
         * this check does not guarantee that the type is correct. Ultimately, this function is unsafe and should be
         * used with caution.
         */
        template<typename T>
        bool TryAddRange(InitialiserList<T> items, bool allOrNothing = false)
        {
            if (items.size() == 0)
                return false;

            OTR_ASSERT_MSG(sizeof(T) == k_Offset, "The type of the buffer does not match the offset of the buffer.")

            const auto listSize = items.size();

            if (listSize > m_Capacity - m_Count)
            {
                if (allOrNothing)
                    return false;

                Expand(listSize - (m_Capacity - m_Count));
            }

            MemorySystem::MemoryCopy((Byte*) m_Data + (m_Count * k_Offset), items.begin(), listSize * k_Offset);
            m_Count += listSize;

            return true;
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
         * @note The T type is checked against the offset of the buffer to ensure that the type is correct. However,
         * this check does not guarantee that the type is correct. Ultimately, this function is unsafe and should be
         * used with caution.
         */
        template<typename T>
        bool TryRemove(const T& item)
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (*reinterpret_cast<T*>((Byte*) m_Data + (i * k_Offset)) == item)
                    return TryRemoveAt(i);

            return false;
        }

        /**
         * @brief Tries to remove an item from the buffer at the specified index.
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
                MemorySystem::MemoryMove((Byte*) m_Data + (index * k_Offset),
                                         (Byte*) m_Data + ((index + 1) * k_Offset),
                                         (m_Count - index - 1) * k_Offset);

            m_Count--;

            return true;
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
         * @brief Used to expand the size of the buffer by a given amount.
         *
         * @param amount The amount to expand the buffer by.
         */
        void Expand(const UInt64 amount = 0)
        {
            UInt64 newCapacity = CalculateExpandCapacity(amount);

            auto handle = Unsafe::New(newCapacity * k_Offset);

            if (IsCreated())
            {
                if (!IsEmpty())
                    MemorySystem::MemoryCopy(handle.Pointer, m_Data, m_Count * k_Offset);

                Unsafe::Delete({ m_Data, m_Capacity * k_Offset });
            }

            m_Data     = handle.Pointer;
            m_Capacity = newCapacity;
        }

        /**
         * @brief Used to shrink the size of the buffer by a given amount.
         *
         * @param amount The amount to shrink the buffer by.
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

            auto handle = Unsafe::New(newCapacity * k_Offset);

            if (IsCreated())
            {
                auto minCount = m_Count < newCapacity
                                ? m_Count
                                : newCapacity;

                MemorySystem::MemoryCopy(handle.Pointer, m_Data, minCount * k_Offset);
                Unsafe::Delete({ m_Data, m_Capacity * k_Offset });
            }

            m_Data     = handle.Pointer;
            m_Capacity = newCapacity;

            if (m_Count >= newCapacity)
                m_Count = newCapacity;
        }

        /**
         * @brief Clears the buffer.
         */
        OTR_INLINE void Clear() { m_Count = 0; }

        /**
         * @brief Clears the buffer and deletes the data.
         */
        void ClearDestructive()
        {
            if (IsCreated())
                Unsafe::Delete({ m_Data, m_Capacity * k_Offset });

            m_Data     = nullptr;
            m_Capacity = 0;
            m_Count    = 0;
        }

#if !OTR_RUNTIME
        /**
         * @brief Gets the memory footprint of the buffer.
         *
         * @param debugName The name of the buffer for debugging purposes.
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
         * @return A pointer to the data of the buffer cast to the specified type.
         *
         * @note The T type is checked against the offset of the buffer to ensure that the type is correct. However,
         * this check does not guarantee that the type is correct. Ultimately, this function is unsafe and should be
         * used with caution.
         */
        template<typename T>
        [[nodiscard]] OTR_INLINE const T* GetData() const noexcept
        {
            OTR_ASSERT_MSG(sizeof(T) == k_Offset, "The type of the buffer does not match the offset of the buffer.")

            return m_Data;
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
        [[nodiscard]] OTR_INLINE UInt64 GetOffset() const noexcept { return k_Offset; }

        /**
         * @brief Checks whether the buffer has been created. A buffer is created when it has been initialised
         * with a valid capacity and has not been destroyed.
         *
         * @return True if the buffer has been created, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept { return m_Data && m_Capacity > 0; }

        /**
         * @brief Checks whether the buffer is empty.
         *
         * @return True if the buffer is empty, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsEmpty() const noexcept { return m_Count == 0; }

    private:
        const UInt64 k_Offset;

        void* m_Data = nullptr;
        UInt64 m_Count    = 0;
        UInt64 m_Capacity = 0;

        /**
         * @brief Recreates the buffer with a given capacity. Deletes any existing data.
         *
         * @param capacity The capacity to recreate the buffer with.
         */
        void RecreateEmpty(const UInt64 capacity)
        {
            if (IsCreated())
                Unsafe::Delete({ m_Data, m_Capacity * k_Offset });

            m_Capacity = capacity;
            m_Count    = 0;

            if (m_Capacity == 0)
                return;

            auto handle = Unsafe::New(m_Capacity * k_Offset);
            m_Data = handle.Pointer;
        }

        /**
         * @brief Calculates the new capacity when expanding the buffer.
         *
         * @param expandAmount The amount to expand the buffer by.
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
         * @param shrinkAmount The amount to shrink the buffer by.
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

#endif //OTTERENGINE_UNSAFEBUFFER_H
