#ifndef OTTERENGINE_DEQUE_H
#define OTTERENGINE_DEQUE_H

#include "Core/Memory.h"
#include "Core/Collections/Iterators/LinearIterator.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    /**
     * @brief A double-ended FIFO (First In First Out) collection. The items are stored in a contiguous memory block
     * on the heap. It does not have a fixed capacity and will expand as needed.
     *
     * @tparam T The type of the items in the deque.
     */
    template<typename T>
    class Deque final
    {
        /// @brief The iterator type for the deque.
        using Iterator = LinearIterator<T>;

        /// @brief The const iterator type for the deque.
        using ConstIterator = LinearIterator<const T>;

    public:
        /**
         * @brief Constructor.
         */
        Deque() = default;

        /**
         * @brief Destructor.
         */
        ~Deque()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);
        }

        /**
         * @brief Creates a deque from an initialiser list.
         *
         * @param list The initialiser list.
         */
        Deque(InitialiserList<T> list)
            : Deque()
        {
            m_Capacity = list.size();
            m_Data     = Buffer::New<T>(m_Capacity);

            m_Count = 0;
            for (const T& item: list)
                m_Data[m_Count++] = item;
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The deque to copy.
         */
        Deque(const Deque<T>& other)
            : Deque()
        {
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Data     = other.m_Data;
        }

        /**
         * @brief Move constructor.
         *
         * @param other The deque to move.
         */
        Deque(Deque<T>&& other) noexcept
            : Deque()
        {
            m_Capacity = std::move(other.m_Capacity);
            m_Count    = std::move(other.m_Count);
            m_Data     = std::move(other.m_Data);

            other.m_Capacity = 0;
            other.m_Count    = 0;
            other.m_Data     = nullptr;
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other The deque to copy.
         *
         * @return A reference to this deque.
         */
        Deque<T>& operator=(const Deque<T>& other)
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Data     = other.m_Data;

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other The deque to move.
         *
         * @return A reference to this deque.
         */
        Deque<T>& operator=(Deque<T>&& other) noexcept
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Capacity = std::move(other.m_Capacity);
            m_Count    = std::move(other.m_Count);
            m_Data     = std::move(other.m_Data);

            other.m_Capacity = 0;
            other.m_Count    = 0;
            other.m_Data     = nullptr;

            return *this;
        }

        /**
         * @brief Pushes an item to the front of the deque.
         *
         * @param value The item to push.
         */
        void PushFront(const T& value)
        {
            if (m_Count >= m_Capacity)
                Expand();

            for (UInt64 i = m_Count; i > 0; i--)
                m_Data[i] = m_Data[i - 1];

            m_Data[0] = value;
            m_Count++;
        }

        /**
         * @brief Pushes an item to the front of the deque.
         *
         * @param value The item to push.
         */
        void PushFront(T&& value)
        {
            if (m_Count >= m_Capacity)
                Expand();

            for (UInt64 i = m_Count; i > 0; i--)
                m_Data[i] = m_Data[i - 1];

            m_Data[0] = std::move(value);
            m_Count++;
        }

        /**
         * @brief Pushes an item to the back of the deque.
         *
         * @param value The item to push.
         */
        void PushBack(const T& value)
        {
            if (m_Count >= m_Capacity)
                Expand();

            m_Data[m_Count++] = value;
        }

        /**
         * @brief Pushes an item to the back of the deque.
         *
         * @param value The item to push.
         */
        void PushBack(T&& value)
        {
            if (m_Count >= m_Capacity)
                Expand();

            m_Data[m_Count++] = std::move(value);
        }

        /**
         * @brief Tries to pop an item from the front of the deque.
         *
         * @return True if an item was popped, false otherwise.
         */
        bool TryPopFront()
        {
            if (m_Count == 0)
                return false;

            for (UInt64 i = 0; i < m_Count - 1; i++)
                m_Data[i] = m_Data[i + 1];

            m_Count--;

            return true;
        }

        /**
         * @brief Tries to pop an item from the front of the deque.
         *
         * @param outItem The item that was popped.
         *
         * @return True if an item was popped, false otherwise.
         */
        bool TryPopFront(T* outItem)
        {
            if (m_Count == 0)
                return false;

            *outItem = m_Data[0];

            for (UInt64 i = 0; i < m_Count - 1; i++)
                m_Data[i] = m_Data[i + 1];

            m_Count--;

            return true;
        }

        /**
         * @brief Tries to pop an item from the back of the deque.
         *
         * @return True if an item was popped, false otherwise.
         */
        bool TryPopBack()
        {
            if (m_Count == 0)
                return false;

            m_Count--;

            return true;
        }

        /**
         * @brief Tries to pop an item from the back of the deque.
         *
         * @param outItem The item that was popped.
         *
         * @return True if an item was popped, false otherwise.
         */
        bool TryPopBack(T* outItem)
        {
            if (m_Count == 0)
                return false;

            *outItem = m_Data[m_Count - 1];
            m_Count--;

            return true;
        }

        /**
         * @brief Tries to peek an item from the front of the deque.
         *
         * @param outItem The item that was peeked.
         *
         * @return True if an item was peeked, false otherwise.
         */
        bool TryPeekFront(T* outItem)
        {
            if (m_Count == 0)
                return false;

            *outItem = m_Data[0];

            return true;
        }

        /**
         * @brief Tries to peek an item from the back of the deque.
         *
         * @param outItem The item that was peeked.
         *
         * @return True if an item was peeked, false otherwise.
         */
        bool TryPeekBack(T* outItem)
        {
            if (m_Count == 0)
                return false;

            *outItem = m_Data[m_Count - 1];

            return true;
        }

        /**
         * @brief Tries to remove an item from the deque.
         *
         * @param item The item to remove.
         *
         * @return True if an item was removed, false otherwise.
         */
        bool TryRemove(const T& item)
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                    return TryRemoveAt(i);

            return false;
        }

        /**
         * @brief Tries to remove an item from the deque.
         *
         * @param item The item to remove.
         *
         * @return True if an item was removed, false otherwise.
         */
        bool TryRemove(T&& item) noexcept
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                    return TryRemoveAt(i);

            return false;
        }

        /**
         * @brief Tries to remove an item from the deque at a given index.
         *
         * @param index The index to remove the item at.
         *
         * @return True if an item was removed, false otherwise.
         */
        bool TryRemoveAt(const UInt64 index)
        {
            if (index >= m_Count)
                return false;

            for (UInt64 i = index; i < m_Count - 1; i++)
                m_Data[i] = m_Data[i + 1];

            m_Count--;
            return true;
        }

        /**
         * @brief Used to reserve space for the deque.
         *
         * @param capacity The capacity to reserve.
         *
         * @note This operation is destructive and will delete any existing data.
         */
        void Reserve(const UInt64 capacity)
        {
            if (capacity <= m_Capacity)
                return;

            T* newData = Buffer::New<T>(capacity);

            for (UInt64 i = 0; i < m_Count; i++)
                newData[i] = m_Data[i];

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data     = newData;
            m_Capacity = capacity;
        }

        /**
         * @brief Used to expand the size of the deque by a given amount.
         *
         * @param amount The amount to expand the deque by.
         */
        void Expand(const UInt64 amount = 0)
        {
            UInt64 newCapacity = CalculateExpandCapacity(amount);

            if (IsEmpty())
            {
                RecreateEmpty(newCapacity);
                return;
            }

            T* newData = Buffer::New<T>(newCapacity);

            for (UInt64 i = 0; i < m_Count; i++)
                newData[i] = m_Data[i];

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data     = newData;
            m_Capacity = newCapacity;
        }

        /**
         * @brief Used to shrink the size of the deque by a given amount.
         *
         * @param amount The amount to shrink the deque by.
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

            T* newData = Buffer::New<T>(newCapacity);

            for (UInt64 i = 0; i < m_Count && i < amount; i++)
                newData[i] = m_Data[i];

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data     = newData;
            m_Capacity = newCapacity;
            m_Count    = m_Count < newCapacity ? m_Count : newCapacity;
        }

        /**
         * @brief Clears the deque.
         */
        OTR_INLINE void Clear() { m_Count = 0; }

        /**
         * @brief Clears the deque and deletes the data.
         */
        void ClearDestructive()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data     = nullptr;
            m_Capacity = 0;
            m_Count    = 0;
        }

#if !OTR_RUNTIME
        /**
         * @brief Gets the memory footprint of the deque.
         *
         * @param debugName The name of the deque for debugging purposes.
         *
         * @return The memory footprint of the deque.
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
         * @brief Gets a pointer to the data of the deque.
         *
         * @return A pointer to the data of the deque.
         */
        [[nodiscard]] OTR_INLINE const T* GetData() const noexcept { return m_Data; }

        /**
         * @brief Gets the capacity of the deque.
         *
         * @return The capacity of the deque.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCapacity() const noexcept { return m_Capacity; }

        /**
         * @brief Gets the item count of the deque.
         *
         * @return The item count of the deque.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCount() const noexcept { return m_Count; }

        /**
         * @brief Checks whether the deque has been created. A deque is created when it has been initialised
         * with a valid capacity and has not been destroyed.
         *
         * @return True if the deque has been created, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept { return m_Data && m_Capacity > 0; }

        /**
         * @brief Checks whether the deque is empty.
         *
         * @return True if the deque is empty, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsEmpty() const noexcept { return m_Count == 0; }

        /**
         * @brief Gets an iterator to the first element of the deque.
         *
         * @return An iterator to the first element of the deque.
         */
        OTR_INLINE Iterator begin() noexcept { return Iterator(m_Data); }

        /**
         * @brief Gets an iterator to the last element of the deque.
         *
         * @return An iterator to the last element of the deque.
         */
        OTR_INLINE Iterator end() noexcept { return Iterator(m_Data + m_Count); }

        /**
         * @brief Gets a reverse iterator to the last element of the deque.
         *
         * @return A reverse iterator to the last element of the deque.
         */
        OTR_INLINE Iterator rbegin() noexcept { return Iterator(m_Data + m_Count - 1); }

        /**
         * @brief Gets a reverse iterator to the first element of the deque.
         *
         * @return A reverse iterator to the first element of the deque.
         */
        OTR_INLINE Iterator rend() noexcept { return Iterator(m_Data - 1); }

        /**
         * @brief Gets a const iterator to the first element of the deque.
         *
         * @return A const iterator to the first element of the deque.
         */
        OTR_INLINE ConstIterator begin() const noexcept { return ConstIterator(m_Data); }

        /**
         * @brief Gets a const iterator to the last element of the deque.
         *
         * @return A const iterator to the last element of the deque.
         */
        OTR_INLINE ConstIterator end() const noexcept { return ConstIterator(m_Data + m_Count); }

        /**
         * @brief Gets a reverse const iterator to the last element of the deque.
         *
         * @return A reverse const iterator to the last element of the deque.
         */
        OTR_INLINE ConstIterator rbegin() const noexcept { return ConstIterator(m_Data + m_Count - 1); }

        /**
         * @brief Gets a reverse const iterator to the first element of the deque.
         *
         * @return A reverse const iterator to the first element of the deque.
         */
        OTR_INLINE ConstIterator rend() const noexcept { return ConstIterator(m_Data - 1); }

    private:
        T* m_Data = nullptr;
        UInt64 m_Capacity = 0;
        UInt64 m_Count    = 0;

        /**
         * @brief Recreates the deque with a given capacity. Deletes any existing data.
         *
         * @param capacity The capacity to recreate the deque with.
         */
        void RecreateEmpty(const UInt64 capacity)
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data     = capacity > 0 ? Buffer::New<T>(capacity) : nullptr;
            m_Capacity = capacity;
            m_Count    = 0;
        }

        /**
         * @brief Calculates the new capacity when expanding the deque.
         *
         * @param expandAmount The amount to expand the deque by.
         *
         * @return The new capacity.
         */
        UInt64 CalculateExpandCapacity(const UInt64 expandAmount)
        {
            UInt64 newCapacity;

            if (expandAmount == 0)
                newCapacity = m_Capacity == 0 ? 2 : m_Capacity * 1.5;
            else
                newCapacity = m_Capacity + expandAmount;

            return newCapacity;
        }

        /**
         * @brief Calculates the new capacity when shrinking the deque.
         *
         * @param shrinkAmount The amount to shrink the deque by.
         * @param isDestructive Whether or not the shrink is destructive. If true, some data may be lost.
         *
         * @return The new capacity.
         */
        UInt64 CalculateShrinkCapacity(const UInt64 shrinkAmount, const bool isDestructive)
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

            if (!isDestructive && newCapacity < m_Count)
                newCapacity = m_Count;

            return newCapacity;
        }
    };
}

#endif //OTTERENGINE_DEQUE_H
